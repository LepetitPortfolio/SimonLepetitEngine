SLE - DIAGNOSTIC TRIANGLE
=========================

This version intentionally removes the normal rendering path from the shader.

Vertex shader:
- does not read the vertex buffer
- does not read the index buffer
- does not read the UBO
- does not read the camera
- generates exactly three clip-space vertices with gl_VertexIndex

Fragment shader:
- ignores textures
- always outputs solid red

C++ command recording:
- uses vkCmdDraw(..., 3, ...) instead of vkCmdDrawIndexed
- the index buffer is not used by the draw

IMPORTANT:
The executable loads Shaders/Vert.spv and Shaders/Frag.spv, not the GLSL files.
Run ShaderCompile.bat before building/running the engine.

EXPECTED RESULT:
A large red triangle centered in the window.

IF RED TRIANGLE APPEARS:
The Vulkan instance/device/swapchain/render pass/framebuffer/pipeline,
viewport, rasterization, fragment stage and presentation path are working.
The problem is then in the normal mesh/vertex/UBO/camera/texture path.

IF WINDOW IS STILL BLACK:
The problem is below the normal mesh path and we should inspect the pipeline,
render pass/framebuffer, command buffer recording, viewport, or actual SPIR-V
being loaded. Do not restore the normal shaders yet; report the exact result.
