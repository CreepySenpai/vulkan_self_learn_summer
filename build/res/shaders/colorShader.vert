#version 460 core

layout(set = 0, binding = 0) uniform TransFormData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} MVPMatrix;

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;


void main(){
    gl_Position = vec4(inVertex, 1.0);

    outColor = inNormal;
    outTexCoord = inTexCoord;
}