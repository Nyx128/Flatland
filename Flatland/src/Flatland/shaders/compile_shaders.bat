@echo off
set shaderDir=C:\dev\3D\Flatland\Flatland\src\Flatland\shaders
set GLSLC="%VULKAN_SDK%\Bin\glslc.exe"
set OUTPUT=C:\dev\3D\Flatland\Flatland\src\Flatland\shaders


CALL %GLSLC% "%shaderDir%\unlit.vert" -o "%OUTPUT%\unlit.vert.spv"
CALL %GLSLC% "%shaderDir%\unlit.frag" -o "%OUTPUT%\unlit.frag.spv"

@echo on
REM shaders compiled successfully
@echo off

