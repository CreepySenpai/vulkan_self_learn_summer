#version 460 core

layout(set = 0, binding = 0) uniform _uniformData{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;

layout(push_constant) uniform _vertexPushConstantData{
    mat4 modelMatrix;
} VertexPushConstantData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uint inEntityID;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) flat out uint outEntityID;

void main(){

    gl_Position = UniformData.projectionMatrix * UniformData.viewMatrix * VertexPushConstantData.modelMatrix * vec4(inPosition, 1.0);

    outPosition = (UniformData.viewMatrix * VertexPushConstantData.modelMatrix * vec4(inPosition, 1.0)).xyz;

    outNormal = (UniformData.viewMatrix * VertexPushConstantData.modelMatrix * vec4(inNormal, 0.0)).xyz;

    outTexCoord = inTexCoord;

    outEntityID = inEntityID;
}