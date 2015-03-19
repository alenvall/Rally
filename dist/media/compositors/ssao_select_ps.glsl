#version 120

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;

uniform vec4 viewportSize; // w, h, 1/w, 1/h in pixels.

void main() {
    vec4 positionSample = texture2D(gbuffer_position, gl_TexCoord[0].xy);
    vec3 position = positionSample.xyz;
    float distance = positionSample.w;
    
    vec4 normalSample = texture2D(gbuffer_normal, gl_TexCoord[0].xy);
    vec3 normal = normalSample.xyz; // Already normalized
    
    float distance2 = texture2D(gbuffer_position, gl_TexCoord[0].xy-viewportSize.zw).w - distance;
    //float distance3 = texture2D(gbuffer_position, gl_TexCoord[0].xy-2*viewportSize.zw).w;
    float distance4 = texture2D(gbuffer_position, gl_TexCoord[0].xy-3*viewportSize.zw).w - distance;
    
    gl_FragColor = vec4(vec3(1, 1, 1)*(distance4/distance2), 1);
}
