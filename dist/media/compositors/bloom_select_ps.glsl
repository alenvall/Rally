#version 120

uniform sampler2D original;

void main() {
    vec4 color = texture2D(original, gl_TexCoord[0].xy);
    
    if(color.r < 0.9 && color.g < 0.9 && color.b < 0.9) {
        color = vec4(0, 0, 0, 1);
    } else {
        color *= color;
    }
    
    gl_FragColor = color;
}
