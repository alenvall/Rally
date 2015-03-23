#version 120

// Hemisphere SSAO. Written with inspiration from Ogre's SSAO demo
// (in particular the Crytek and Hemisphere modes), and some insights from
// http://john-chapman-graphics.blogspot.se/2013/01/ssao-tutorial.html

// Projection matrix as in how the scene was rendered, not what the compositor uses.
uniform mat4 sceneProjectionMatrix;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;

uniform float effectFactor;

void main() {
    // A script to generate the random hemisphere vectors
    // Array.apply(null, Array(16)).map(function(){ var a = 2*Math.random()-1; var b = 2*Math.random()-1; var c = Math.random(); var len = Math.sqrt(a*a + b*b + c*c); return [a/len, b/len, c/len]; }).map(function(vec){ var rand = Math.random(); rand *= rand; /* scatter points nearer the base */ return [rand*vec[0], rand*vec[1], rand*vec[2]]; }).map(function(vec){ return "vec3(" + vec[0] + ", " + vec[1] + ", " + vec[2] + ")"; }).join(",\n")
    // It takes a randomized vector [-1, 1]x[-1, 1]x[0, 1], normalizes it and then scales it by a value [0, 1].
    // The scale factor is choosen randomly from a distribution with PDF(x) = x*x.
    // This leads to, statistically, more points near the base of the hemisphere (quadratically).
    // As can be seen, the z-values are never negative, this gives our hemisphere.
    // All that is needed is rotating the hemisphere with the normal.
    vec3 hemisphere[16] = vec3[](
        vec3(0.0020756480994042702, -0.017572747806015608, 0.05995900403090007),
        vec3(0.21463696030158566, -0.3067030745792103, 0.8721968211060579),
        vec3(0.014071842137666575, -0.013840455314652373, 0.03494547217856626),
        vec3(0.014252748707934144, -0.005255269773646369, 0.006892984633250459),
        vec3(-0.0004377977026165935, 0.0006632623636295884, 0.0006937579736438057),
        vec3(0.08199710488714318, -0.020202363977663958, 0.10241137694052568),
        vec3(-0.23016410389435638, 0.8255269031439877, 0.2511641670259204),
        vec3(-0.05864085214779092, -0.02155261119608473, 0.297979711405229),
        vec3(-0.03300579964400299, -0.024575805980712958, 0.02089384282231312),
        vec3(0.29587371774236826, 0.041955615534282203, 0.12080975606603449),
        vec3(0.21500669719719034, -0.35010532550215095, 0.28574103836787157),
        vec3(-0.18798686309681095, -0.1476248970971063, 0.07626244346614072),
        vec3(0.02158442615806525, -0.042233830338288175, 0.03328912327506663),
        vec3(0.11231829746147552, -0.07222822529709923, 0.0714465029581088),
        vec3(0.07776175312312252, -0.015005833608012735, 0.47533973346899466),
        vec3(-0.0012428876030767958, 0.0020953290764394185, 0.0002652865254757231)
    );

    vec3 position = texture2D(gbuffer_position, gl_TexCoord[0].xy).xyz;
    vec3 normal = texture2D(gbuffer_normal, gl_TexCoord[0].xy).xyz; // Already normalized
    
    float radius = effectFactor; // Needs to be inside what we can blur obviously...
    
    float occlusion = 0.0;
    for(int i = 0; i < 16; ++i) {
        vec3 randomVector = radius * hemisphere[i];
        
        // Project the randomized vector onto a thought x-axis in the plane 'normal' is orthogonal to
        vec3 thoughtX = normalize(randomVector - normal * dot(normal, randomVector));
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
            vec4(position.x, -position.y /* TODO: Why does this work better (does it?)? */, position.z, 1.0));
        
        // Now, just project the position onto screen space so that we can sample!
        vec4 sampleProbedPosition = sampleModelView * vec4(randomVector, 1.0);
        vec4 sampleCoord = sceneProjectionMatrix * sampleProbedPosition;
        
        // Perspective divide and transtation [-1, 1] -> [0, 1] (NDC -> texcoord)
        sampleCoord.xy = vec2(0.5, 0.5) + 0.5*(sampleCoord.xy / sampleCoord.w);
        
        vec3 sampleRealPosition = texture2D(gbuffer_position, sampleCoord.xy).xyz;
        
        // Only count if this pixel was in fact behind the (guessed) sampled pixel in the scene (meaning it was occluded).
        // Also, filter away stuff that is too far away (outside our hemisphere), adding some realism.
        if(sampleRealPosition.z >= sampleProbedPosition.z && length(sampleRealPosition - sampleProbedPosition.xyz) <= radius) {
            occlusion += 1.0;
        }
    }
    
    float occlusionColor = 1.0 - (occlusion / 16.0);
    gl_FragColor = vec4(normal, 1.0);
}
