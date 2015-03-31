#version 120

uniform sampler2D original;
uniform sampler2D gbuffer_lighting;

void main() {
    vec4 color = texture2D(original, gl_TexCoord[0].xy);
    float emissiveStrength = texture2D(gbuffer_lighting, gl_TexCoord[0].xy).w;
    
    // Extra-brighten the bright colors, then modulate with the emisive strength
    // (Most materials have an emissiveStength = emissive.(r+g+b) = 0.)
    color.rgb = color.rgb * color.rgb * clamp(emissiveStrength, 0.0, 1.0);
    
    gl_FragColor = color;
}
