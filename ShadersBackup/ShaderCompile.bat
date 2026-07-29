@echo off

set VulkanDir=D:/Logiciels/VulkanSDK/1.4.341.1
set VertexScriptDir=4_shader.vert
set OutVertexName=4_vert.spv
set FragmentScriptDir=4_shader.frag
set OutFragmentName=4_frag.spv

@echo on

call %VulkanDir%/Bin/glslc.exe %VertexScriptDir% -o %OutVertexName%
call %VulkanDir%/Bin/glslc.exe %FragmentScriptDir% -o %OutFragmentName%

pause