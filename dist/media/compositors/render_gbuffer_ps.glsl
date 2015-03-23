#version 120
#extension GL_ARB_draw_buffers : enable
// With GL_ARB_draw_buffers, we draw into the gl_FragData[n] arrays instead
// of the gl_FragColor vector.

// Based on Ogre's SSAO demo (uses a G buffer)

uniform float nearClipDistance;
uniform float farClipDistance;

varying vec3 viewSpacePosition;
varying vec3 viewSpaceNormal;

void main() {
    // Need to renormalize the interpolated value
    vec3 normalizedNormal = normalize(viewSpaceNormal);
    
    // Everything is clipped and in viewspace so this works, with the
    // small exception that the distance is from the middle point of the near
    // clipping plane relative to the viewing frustum, not a straight ray.
    //float distance = length(viewSpacePosition) - nearClipDistance;
    //float normalizedDistance = distance / (farClipDistance - nearClipDistance);
    
    gl_FragData[0] = vec4(viewSpacePosition, 0.0);//normalizedDistance);
    gl_FragData[1] = vec4(normalizedNormal, 0.0);
}

