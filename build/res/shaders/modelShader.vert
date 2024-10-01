#version 460 core

#extension GL_EXT_buffer_reference : require

layout(set = 0, binding = 0) uniform _transformData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} TransFormData;

layout(push_constant) uniform _vertexPushConstantData{
    mat4 modelTransformData;
} VertexPushConstantData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) flat out vec3 outCameraPosition;

void main(){
    mat4 mvpMatrix = TransFormData.projectionMatrix * TransFormData.viewMatrix * TransFormData.modelMatrix;

    vec4 loc = mvpMatrix * VertexPushConstantData.modelTransformData * vec4(inPosition, 1.0);
    gl_Position = loc;

    // Position In World Space
    outPosition = (TransFormData.modelMatrix * vec4(inPosition, 1.0)).xyz;

    // Normal In World Space
    outNormal = mat3(TransFormData.modelMatrix) * inNormal;
    outTexCoord = inTexCoord;
    
    // TODO: Change to view space
    // outCameraPosition = TransFormData.viewMatrix * TransFormData.modelMatrix * TransFormData.cameraPosition.xyz;
    outCameraPosition = TransFormData.cameraPosition.xyz;
}