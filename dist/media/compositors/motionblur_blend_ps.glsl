#version 120

uniform sampler2D original;
uniform sampler2D occlusionMap;

void main() {
    vec2 uvCoords = gl_TexCoord[0].xy;
    vec4 color = texture2D(original, uvCoords);
    
    // This value is already prescaled with the effectfactor etc. in ssao_select_ps
    float edgeDetection = texture2D(occlusionMap, gl_TexCoord[0].xy).b;
    
    // Shift the start to 0.5, then increase rapidly.
    edgeDetection = (edgeDetection > 0.4) ? (edgeDetection - 0.4)/(1.0 - 0.4) : 0.0;
    edgeDetection = clamp(4.0 * edgeDetection, 0.0, 1.0);
    
    // assert(edgeDetection is between 0.0 and 1.0) should hold here.
    
    // Be careful! Only use an alpha > 0.5, lower values might look good but it
    // also means everything lags behind one frame. Then instead use
    // goodValue = 1.0 - looksQuiteNiceAlpha, it will look the same but is more
    // up-to-date with the current frame.
    float falloff = 3.0*length(uvCoords - vec2(0.5, 0.5));
    color.a = 1.0 - 0.3 * edgeDetection * falloff*falloff;
    
    gl_FragColor = color;
}
