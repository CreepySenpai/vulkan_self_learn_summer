glslc modelShader.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o modelVertexShader.spv
glslc modelShader.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o  modelFragmentShader.spv

glslc skybox.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o skyBoxVert.spv
glslc skybox.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o skyBoxFrag.spv

glslc quadShader.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o quadShaderVert.spv
glslc quadShader.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o quadShaderFrag.spv