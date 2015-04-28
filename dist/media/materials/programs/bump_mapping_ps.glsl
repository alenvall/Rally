#version 120

uniform vec4 lightDiffuse;
uniform sampler2D normalMap;

varying vec4 oUv0;
varying vec3 oTSLightDir;

void main() {
	// Get bump map normal
	vec3 bumpVec = 2.0*texture2D(normalMap, oUv0.xy).xyz - 1.0;

	gl_FragColor = lightDiffuse * dot(bumpVec, normalize(oTSLightDir).xyz);
}
