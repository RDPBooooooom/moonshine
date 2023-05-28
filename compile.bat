set shaderFolder=%1
set outputFolder=%2


echo Compiling vertex shader...
%VK_SDK_PATH%\Bin\glslc.exe %shaderFolder%shader.vert -o %outputFolder%shader.vert.spv

echo Compiling fragment shader...
%VK_SDK_PATH%\Bin\glslc.exe %shaderFolder%shader.frag -o %outputFolder%shader.frag.spv

echo Done compiling shaders.
