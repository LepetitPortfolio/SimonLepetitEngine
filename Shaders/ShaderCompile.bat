@echo off
setlocal

set VulkanDir=D:/Logiciels/VulkanSDK/1.4.341.1
set OutFileDir=../out/build/Win-x64-debug/Shaders/


if "%VULKAN_SDK%"=="D:/Logiciels/VulkanSDK/1.4.341.1" (
    echo ERROR: VULKAN_SDK is not defined.
    echo Install the Vulkan SDK and make sure VULKAN_SDK points to it.
    pause
    exit /b 1
)

pushd "%~dp0"

"%VULKAN_SDK%\Bin\glslc.exe" shader.vert -o %OutFileDir%Vert.spv
if errorlevel 1 goto :error

"%VULKAN_SDK%\Bin\glslc.exe" shader.frag -o %OutFileDir%Frag.spv
if errorlevel 1 goto :error

echo.
echo ========================================
echo Diagnostic shaders compiled successfully.
echo ========================================
popd
pause
exit /b 0

:error
echo.
echo ERROR: Shader compilation failed.
popd
pause
exit /b 1
