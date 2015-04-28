#version 120

uniform sampler2D original;
uniform sampler2D ssaoMap;

void main() {
    vec3 ssaoInfo = texture2D(ssaoMap, gl_TexCoord[0].xy).rgb;
    vec4 originalColor = texture2D(original, gl_TexCoord[0].xy);
    
    vec3 ssaoShadows = ssaoInfo.r * vec3(1.0, 1.0, 1.0);
    
    //gl_FragColor = vec4(ssaoColor, 1.0);
    gl_FragColor = vec4(ssaoShadows, 1.0)*originalColor;
}
