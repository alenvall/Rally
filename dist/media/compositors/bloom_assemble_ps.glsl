#version 120

uniform sampler2D original;
uniform sampler2D bloomMap;

void main() {
    vec3 bloomColor = texture2D(bloomMap, gl_TexCoord[0].xy).rgb;
    vec4 originalColor = texture2D(original, gl_TexCoord[0].xy);
    vec4 bloomFactor = (vec4(1.0, 1.0, 1.0, 1.0) + vec4(bloomColor, 0));
    
    //gl_FragColor = vec4(bloomColor, 0);
    gl_FragColor = bloomFactor*originalColor + 0.5*vec4(bloomColor, 0);
}
