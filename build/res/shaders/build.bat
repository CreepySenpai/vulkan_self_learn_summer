glslc colorShader.vert --target-spv=spv1.6 -o vertexShader.spv
glslc colorShader.frag  --target-spv=spv1.6 -o  fragmentShader.spv

rem glslc test.vert -o tesst.spv