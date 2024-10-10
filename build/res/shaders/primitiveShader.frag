#version 460 core

#extension GL_EXT_buffer_reference : require

layout(set = 0, binding = 0) uniform _uniformData{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;

layout(set = 1, binding = 0) uniform samplerCube cubeMap;

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer LightBuffer{
    vec4 lightPosition;
    vec4 lightAmbientColor;
    vec4 lightDiffuseIntensity;
    vec4 lightIntensity;
};

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer MaterialBuffer{
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
};

layout(push_constant) uniform _fragmentPushConstant{
    layout(offset = 64) LightBuffer lightBuffer;
    layout(offset = 72) MaterialBuffer materialBuffer;
    layout(offset = 80) uint diffuseTextureID;
} FragmentPushConstantData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) flat in uint inEntityID;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outEntityID;

vec4 frogxyProxy(in vec3 currentColor, in vec3 vertexPosition){
    const vec3 frogColor = vec3(0.2, 0.2, 0.2);
    const float minFrogDis = 3.0;
    const float maxFrogDis = 30.0;

    const float distanceFromCam = length(-vertexPosition);

    float frogFactor = (distanceFromCam - minFrogDis) / (maxFrogDis - minFrogDis);
    frogFactor = clamp(frogFactor, 0.0, 1.0);

    return vec4(mix(currentColor, frogColor, frogFactor), 1.0);
}

vec3 enviromentReflect(in vec3 position, in vec3 normal){
    vec3 reflectVec = reflect(position, normal);
    reflectVec = (inverse(UniformData.viewMatrix) * vec4(reflectVec, 0.0)).xyz;
    return reflectVec;
}

vec3 enviromentRefract(in vec3 position, in vec3 normal){
    const float ratio = 1.0 / 1.33333;
    vec3 refractVec = refract(position, normal, ratio);
    refractVec = (inverse(UniformData.viewMatrix) * vec4(refractVec, 0.0)).xyz;
    return refractVec;
}

void main(){
    // outColor = texture(cubeMap, enviromentRefract(normalize(inPosition), normalize(inNormal)));

    outColor = texture(cubeMap, enviromentReflect(normalize(inPosition), normalize(inNormal)));

    outColor = frogxyProxy(outColor.xyz, inPosition);

    outEntityID = inEntityID;
}