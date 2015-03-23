#version 120

// Based on Ogre's SSAO demo (uses a G buffer)

uniform mat4 worldViewProj;
uniform mat4 worldView;

varying vec3 viewSpacePosition;
varying vec3 viewSpaceNormal;

void main() {
    viewSpacePosition = (worldView*gl_Vertex).xyz;
    viewSpaceNormal = (worldView * vec4(gl_Normal, 0)).xyz;
    //viewSpaceNormal = (mat3(transpose(inverse(worldView))) * gl_Normal).xyz;
    
    gl_Position = worldViewProj * gl_Vertex;
}

