#version 460 core

layout(set = 0, binding = 0) uniform _uniformData{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main(){
    gl_Position = vec4(inPosition, 1.0);

    outTexCoord = inTexCoord;
}