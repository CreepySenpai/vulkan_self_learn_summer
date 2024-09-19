#version 460 core


layout(set = 1, binding = 0) uniform samplerCube cubeMap;

layout(location = 0) in vec3 inTexCoord;

layout(location = 0) out vec4 outColor;

void main(){
    outColor = texture(cubeMap, inTexCoord);
}