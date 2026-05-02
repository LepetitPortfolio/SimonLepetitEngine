@echo off

set VulkanDir=D:/Logiciels/VulkanSDK/1.4.341.1
set VertexScriptDir=3_shader.vert
set OutVertexName=3_vert.spv
set FragmentScriptDir=3_shader.frag
set OutFragmentName=3_frag.spv

@echo on

call %VulkanDir%/Bin/glslc.exe %VertexScriptDir% -o %OutVertexName%
call %VulkanDir%/Bin/glslc.exe %FragmentScriptDir% -o %OutFragmentName%

pause