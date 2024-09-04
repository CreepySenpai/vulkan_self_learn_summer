#version 460 core

#extension GL_EXT_buffer_reference : require

layout(set = 0, binding = 0) uniform _transformData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} TransFormData;

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer LightBuffer{
    vec4 lightPosition;
    vec4 ambientColor;
    vec4 lightIntensity;
};

layout(push_constant) uniform _pushConstantData{
    mat4 modelTransformData;
    LightBuffer lightBuffer;
} PushConstantData;

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main(){
    mat4 MVPMatrix = TransFormData.projectionMatrix * TransFormData.viewMatrix * TransFormData.modelMatrix;
    vec4 loc = MVPMatrix * PushConstantData.modelTransformData * vec4(inVertex, 1.0);
    gl_Position = loc;

    outColor = inNormal;
    outTexCoord = inTexCoord;
}