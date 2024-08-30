#version 460 core

layout(set = 0, binding = 0) uniform _transformData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} TransFormData;

layout(set = 0, binding = 1) uniform _lightData{
    vec4 lightPosition;
    vec4 ambientColor;
    vec4 lightIntensity;
} LightData;

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

layout(push_constant) uniform _preTransformData{
    mat4 modelTransformData;
} PreTransformData;


void main(){
    mat4 MVPMatrix = TransFormData.projectionMatrix * TransFormData.viewMatrix * TransFormData.modelMatrix;
    vec4 loc = MVPMatrix * PreTransformData.modelTransformData * vec4(inVertex, 1.0);
    gl_Position = loc;

    outColor = inNormal;
    outTexCoord = inTexCoord;
}