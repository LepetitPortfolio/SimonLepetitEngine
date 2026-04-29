@echo off

set VulkanDir=D:/Logiciels/VulkanSDK/1.4.341.1
set VertexScriptDir=shader.vert
set OutVertexName=vert.spv
set FragmentScriptDir=shader.frag
set OutFragmentName=frag.spv

@echo on

call %VulkanDir%/Bin/glslc.exe %VertexScriptDir% -o %OutVertexName%
call %VulkanDir%/Bin/glslc.exe %FragmentScriptDir% -o %OutFragmentName%

pause