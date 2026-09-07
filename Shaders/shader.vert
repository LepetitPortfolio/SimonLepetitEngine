#version 450

// Diagnostic vertex shader:
// Deliberately ignores vertex buffers, indices, camera matrices and textures.
// If this triangle is visible, the Vulkan render/present path is working and
// the remaining problem is in the normal mesh/UBO/vertex path.

vec2 positions[3] = vec2[](
    vec2(-0.60, -0.60),
    vec2( 0.60, -0.60),
    vec2( 0.00,  0.60)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
