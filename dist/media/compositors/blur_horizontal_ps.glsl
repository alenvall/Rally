uniform vec4 weights; // See also baseWeight below
uniform vec4 offsets;

uniform sampler2D unblurred;

uniform float baseWeight;

void main() {
    float x = gl_TexCoord[0].x;
    float y = gl_TexCoord[0].y;

    // Add weights 4 left (-), middle (0), 4 right (+)
    // The offsets adds an additional blend between two pixels
    vec3 blurred = baseWeight * texture2D(unblurred, vec2(x, y)).rgb;
    for(int i = 0; i < 4; i++) {
        blurred += weights[i] * texture2D(unblurred, vec2(x + offsets[i], y)).rgb;
        blurred += weights[i] * texture2D(unblurred, vec2(x - offsets[i], y)).rgb;
    }
    
    gl_FragColor = vec4(blurred, 1.0);
}
