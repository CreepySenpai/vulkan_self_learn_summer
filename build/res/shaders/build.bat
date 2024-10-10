glslc modelShader.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o modelVertexShader.spv
glslc modelShader.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o  modelFragmentShader.spv

glslc skybox.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o skyBoxVert.spv
glslc skybox.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o skyBoxFrag.spv

glslc primitiveShader.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o primitiveShaderVert.spv
glslc primitiveShader.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o primitiveShaderFrag.spv


glslc gridShader.vert -O --target-env=vulkan1.3 --target-spv=spv1.6 -o gridShaderVert.spv
glslc gridShader.frag -O --target-env=vulkan1.3 --target-spv=spv1.6 -o gridShaderFrag.spv