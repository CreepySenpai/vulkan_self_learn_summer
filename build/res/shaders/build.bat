glslc modelShader.vert --target-spv=spv1.6 -o modelVertexShader.spv
glslc modelShader.frag  --target-spv=spv1.6 -o  modelFragmentShader.spv

glslc skybox.vert --target-spv=spv1.6 -o skyBoxVert.spv
glslc skybox.frag --target-spv=spv1.6 -o skyBoxFrag.spv