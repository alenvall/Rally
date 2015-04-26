#version 120

/* Bump mapping vertex program
   In this program, we want to calculate the tangent space light vector
   on a per-vertex level which will get passed to the fragment program,
   or to the fixed function dot3 operation, to produce the per-pixel
   lighting effect. 
*/
// parameters
uniform vec4 lightPosition; // object space
uniform mat4 worldViewProj;
uniform float lightNumber; // counts up for each once_per_light: 1, 2, 3...

attribute vec4 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec4 uv0;

varying vec4 oUv0;
varying vec3 oTSLightDir;

void main() {
	// Calculate output position
	gl_Position = worldViewProj * vertex;
	gl_Position.z += 0.001*(1.0 + lightNumber);

	// Pass the main uvs straight through unchanged
	oUv0 = uv0;

	// Calculate tangent space light vector
	// Get object space light direction
	// Non-normalised since we'll do that in the fragment program anyway
	vec3 lightDir = lightPosition.xyz - (vertex * lightPosition.w).xyz;

	// Use the Gram-Schmidt-process to get back to a truly orthonormal base.
	// (The lerp gl does makes the interpolated tangent a linear combination of
	// mostly the true tanegnt, but also some of the normal.)
	vec3 trueTangent = normalize(tangent - dot(normal, tangent)*tangent);
	
	vec3 binormal = cross(normal, trueTangent);

	// Form a rotation matrix out of the vectors, column major for glsl
	mat3 rotation = mat3(vec3(trueTangent[0], binormal[0], normal[0]),
						vec3(trueTangent[1], binormal[1], normal[1]),
						vec3(trueTangent[2], binormal[2], normal[2]));
	
	// Transform the light vector according to this matrix
	oTSLightDir = rotation * lightDir;
}
