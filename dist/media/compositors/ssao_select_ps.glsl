#version 120

// Hemisphere SSAO. Written with inspiration from Ogre's SSAO demo
// (in particular the Crytek and Hemisphere modes), and some insights from
// http://john-chapman-graphics.blogspot.se/2013/01/ssao-tutorial.html

// Projection matrix as in how the scene was rendered, not what the compositor uses.
uniform mat4 sceneProjectionMatrix;

uniform vec4 viewportSize;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler1D random_hemisphere;

uniform float effectFactor;

void main() {
    // A script to generate the random hemisphere vectors
    // (function(nearDist, farDist){ return Array.apply(null, Array(16)).map(function(){ var a = 2*Math.random()-1; var b = 2*Math.random()-1; var c = Math.random(); var len = Math.sqrt(a*a + b*b + c*c); return [a/len, b/len, c/len]; }).map(function(vec){ var rand = Math.random(); rand *= rand; rand = nearDist + (farDist-nearDist)*rand; return [rand*vec[0], rand*vec[1], rand*vec[2]]; }).map(function(vec){ return "vec3(" + vec[0] + ", " + vec[1] + ", " + vec[2] + ")"; }).join(",\n        "); })(0.1, 0.5);
    // It takes a randomized vector [-1, 1]x[-1, 1]x[0, 1], normalizes it and then scales it by a value [1, 4].
    // As can be seen, the z-values are never negative, this gives our hemisphere.
    // The scale factor is choosen randomly from a distribution with PDF(x) = nearDist + (farDist-nearDist)*x*x.
    // This leads to, statistically, more points near the base of the hemisphere (quadratically).
    // The length is bounded by [1, 4] and the idea is that it should be scaled further so that
    // no texel will test itself for occlusion. A cheap way is to just change radius below and
    // hope for the best, even if the results wouldn't be 100 % accurate.
    /*const vec3 hemisphere[16] = vec3[](
        vec3(-0.17947426117077606, -0.2552789504219729, 0.32771201703308744),
        vec3(-0.0929430250903943, -0.15022899751515606, 0.15734721849596883),
        vec3(-0.023182541661165706, 0.08987930735474939, 0.06378498765102361),
        vec3(-0.2504982585129548, -0.21735898345747406, 0.19624367519245178),
        vec3(-0.020959529911299728, -0.340341322226052, 0.04663782253381715),
        vec3(0.06614578641177501, -0.06934635075715916, 0.03723163252854354),
        vec3(0.11051493486728853, -0.3075720427417002, 0.20906022960369047),
        vec3(0.19509182324257024, 0.10203328121782702, 0.07510483504565645),
        vec3(0.12781646826356982, -0.3167203611463459, 0.33289697509920874),
        vec3(-0.12221070092220723, -0.03258109258316044, 0.24833611736630345),
        vec3(0.08159243530451434, 0.10596728486347019, 0.10018306171759324),
        vec3(0.059999509667150175, -0.08176483505414722, 0.07445634853488346),
        vec3(0.06621578268766325, -0.17033266961717766, 0.21850057312155527),
        vec3(-0.04004902358906149, -0.07537300385054206, 0.07790280988401853),
        vec3(-0.3014783919444156, -0.21485240026169983, 0.08698036845677733),
        vec3(0.11120756426123887, -0.13074617072995254, 0.03665566895440695)
    );*/

    // Without const for the hemisphere array: horrible performance. With const: does not work on all cards (non-standard)
    // The values were converted with the formula Math.round(127.5 + vector*127.5), yielding:
    /*
    105 95 169
    116 108 148
    125 139 136
    96 100 153
    125 84 133
    136 119 132
    142 88 154
    152 141 137
    144 87 170
    112 123 159
    138 141 140
    135 117 137
    136 106 155
    122 118 137
    89 100 139
    142 111 132
    */
    // These values were specifiedd as xyz = rgb, and saved into a 16x1 texture (not gamma corrected).

    const float farDist = 0.5;

    vec3 position = texture2D(gbuffer_position, gl_TexCoord[0].xy).xyz;
    vec3 normal = texture2D(gbuffer_normal, gl_TexCoord[0].xy).xyz; // Already normalized
    
    // Get some high-frequency noise used below, use the current pixel index and
    // do a modulo 4 on it, so that we get 0, 1, 2, 3, 0, 1, 2, 3.
    vec2 highFrequencyNoise = viewportSize.xy * gl_TexCoord[0].xy;
    highFrequencyNoise = highFrequencyNoise - 4.0*floor(
        0.0001 + // Get rid of rounding errors, as we might not exactly get 0, 1, 2, ...
        highFrequencyNoise/4.0
    );
    // We check if it's >= 4 and clamp to 3 (4 might appear as a roundig issue).
    // Also, we must remap so that we get equally much randomness in one direction
    // than the other, otherwise the shadows will appear different when viewed
    // from opposite directions.
    // 0 -> -1.5, 1 -> -0.5, 2 -> 0.5, 3 -> 1.5
    highFrequencyNoise -= 1.5;
    if(highFrequencyNoise.x >= 1.5) highFrequencyNoise.x = 1.5;
    if(highFrequencyNoise.y >= 1.5) highFrequencyNoise.y = 1.5;
    
    // Finally, add 0.1 (we don't want a 0-vector below), and normalize.
    highFrequencyNoise = normalize(0.1 + highFrequencyNoise);
    
    // Get any vector colinear with the surface normal and cross it to get an X-axis.
    // Then cross that with the normal again to get an Y axis. We might choose about
    // any vector, as long as it isn't colinear with the normal. Let's introduce some
    // randomness here, by rotating the coordinate system we form around the normal.
    // This high-frequence noise can be low-pass filtered away in a blurring stage.
    // Going this noise+blur decreases banding effects on the surfaces.
    vec3 randomness;
    
    // Choose the vector component that is biggest and make 0. Since the normal is
    // normalized, we have a great chance at finding something non-colinear.
    // randomness may be regarded as normalized, as highFrequencyNoise is that
    // and one of randomness' components is 0.
    vec3 normalAbs = abs(normal);
    if(normalAbs.x > normalAbs.y && normalAbs.x > normalAbs.z) {
        randomness = vec3(0.0, highFrequencyNoise.x, highFrequencyNoise.y);
    } else if (normalAbs.y > normalAbs.x && normalAbs.y > normalAbs.z) {
        randomness = vec3(highFrequencyNoise.x, 0.0, highFrequencyNoise.y);
    } else {
        // Not catching solutions around x = y = z is acceptable (fuzzy =).
        randomness = vec3(highFrequencyNoise.x, highFrequencyNoise.y, 0.0);
    }
    
    vec3 thoughtX = cross(normal, randomness);
    vec3 thoughtY = cross(normal, thoughtX);
    
    // Building a rotation matrix column by column, seeing where each basis vector ends up.
    // We want the Z-axis to originate where the normal points, as all the pre-computed
    // vectors face that way in some sense (z > 0 always). This makes the hemisphere
    // face the right way as if its 'up' vector/bottom plane normal was oriented that way.
    // Also, add the position, that will effectively translate the randomVector into the
    // correct spot in the scene (world space).
    mat4 sampleModelView = mat4(
        vec4(thoughtX, 0.0),
        vec4(thoughtY, 0.0),
        vec4(normal, 0.0),
        vec4(position.x, position.y, position.z, 1.0));
        
    float occlusion = 0.0;
    float edgeDetection = 0.0;
    for(int i = 0; i < 16; ++i) {
        vec3 hemisphereLocalSamplePosition = 2.0*texture1D(random_hemisphere, (0.5/16.0) + (1.0/16.0)*i).rgb - 1.0;
        
        // Now, just project the position onto screen space so that we can sample!
        vec4 sampleProbedPosition = sampleModelView * vec4(hemisphereLocalSamplePosition, 1.0);
        vec4 sampleCoord = sceneProjectionMatrix * sampleProbedPosition;
        
        // Perspective divide and transtation [-1, 1] -> [0, 1] (NDC -> texcoord)
        sampleCoord.xy = vec2(0.5, 0.5) + 0.5*(sampleCoord.xy / sampleCoord.w);
        sampleCoord.y = 1.0 - sampleCoord.y;
        vec3 sampleRealPosition = texture2D(gbuffer_position, sampleCoord.xy).xyz;
        
        float distance = abs(sampleProbedPosition.z - sampleRealPosition.z);
        
        // Only count if this sampled pixel was in fact in front of the (guessed) probed pixel
        // (meaning it was occluding the current pixel in the rendered scene).
        if(sampleRealPosition.z >= sampleProbedPosition.z) {
            float occlusionAdd = 1.0;
            
            // Filter off stuff that is too far away (outside our hemisphere),
            // for example edges of houses etc. (The only differ in their z-coord).
            if(distance > farDist) {
                // Provide some falloff, so that the shadow doesn't suddenly disappear.
                occlusionAdd = clamp(/* works better without for some reason: farDist* */ farDist/(distance*distance), 0.0, 1.0);
                edgeDetection += 1.0 - occlusionAdd;
            }
            
            // The sky box is off the GBuffer, but otherwise we could filter away
            // the SSAO from it here as it seems to interfere with it.
            occlusion += occlusionAdd;
        } else if(distance > farDist) {
            edgeDetection += 1.0;
        }
    }
    
    float occlusionColor = 1.0 - (occlusion / 16.0);
    
    // This is shared with the motion blur compositor
    float edgeDetectionForMotionBlur = effectFactor * (0.5 + 0.5 * edgeDetection / 16.0); // Used in the motionblur_blend shader
    gl_FragColor = vec4(occlusionColor, edgeDetection, edgeDetectionForMotionBlur, 1.0);
}
