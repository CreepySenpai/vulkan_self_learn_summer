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

void main(){
    mat4 mvpMatrix = TransFormData.projectionMatrix * TransFormData.viewMatrix * TransFormData.modelMatrix;
    
    // 
    mat3 modelViewMatrix = mat3(TransFormData.viewMatrix * TransFormData.modelMatrix);
    // vec3 normal = normalize(NormalMatrix * inNormal);

    // vec4 camCoord = MVPMatrix * vec4(inVertex, 1.0);
    // vec3 s = normalize(vec3(PushConstantData.lightBuffer.lightPosition - camCoord));
    // float sDotN = max(dot(s, normal), 0.0);

    // vec3 diffuse = vec3(PushConstantData.lightBuffer.diffuseIntensity * sDotN);
    // vec3 specular = vec3(0.0);

    // if(sDotN > 0.0) {
    //     vec3 v = normalize(-camCoord.xyz);
    //     vec3 r = reflect(-s, normal);
    //     specular = vec3(PushConstantData.lightBuffer.lightIntensity * pow(max(dot(r, v), 0.0), 25.0));
    // }

    vec4 loc = mvpMatrix * VertexPushConstantData.modelTransformData * vec4(inPosition, 1.0);
    gl_Position = loc;

    // Position In Camera Space
    outPosition = modelViewMatrix * inPosition;
    // Normal In Camera Space
    outNormal = modelViewMatrix * inNormal;
    // outColor = vec3(PushConstantData.lightBuffer.ambientColor + vec4(diffuse, 1.0) + vec4(specular, 1.0));
    outTexCoord = inTexCoord;
}