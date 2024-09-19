#version 460 core

layout(set = 0, binding = 0) uniform _transformData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} TransFormData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outTexCoord;

void main(){
    vec3 t = inNormal;
    vec2 tt = inTexCoord;

    outTexCoord = inPosition;
    
    // Remove translation
    mat4 newModelMatrix = mat4(mat3(TransFormData.modelMatrix));
    gl_Position = TransFormData.projectionMatrix * TransFormData.viewMatrix * newModelMatrix * vec4(inPosition, 1.0);
}