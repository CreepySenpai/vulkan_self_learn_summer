#version 460 core

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

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

layout(location = 0) in vec3 inPosition;    // Position Eye Space
layout(location = 1) in vec3 inNormal;      // Normal Eye Space
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) flat in uint inEntityID;

// Uniform Var

layout(set = 0, binding = 0) uniform _uniformData{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;

layout(set = 1, binding = 0) uniform sampler2D myTextures[];

layout(location = 0) out vec4 finalColor;
layout(location = 1) out uint finalEntityID;

const uint specularExponent = 100; 

vec3 phongLightModel(in vec3 vertexPosition, in vec3 normal, in vec3 lightVec){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const float lambertian = max(dot(lightVec, normal), 0.0);

    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * lambertian);

    const vec3 reflectVec = reflect(-lightVec, normal);

    const float specularFactor = pow(max(dot(reflectVec, normalize(-vertexPosition)), 0.0), specularExponent);
    
    const vec3 specular = (FragmentPushConstantData.lightBuffer.lightIntensity * FragmentPushConstantData.materialBuffer.materialSpecular * specularFactor).xyz;

    return ambient + diffuse + specular;
}

vec3 blindPhongLighModel(in vec3 vertexPosition, in vec3 normal, in vec3 lightVec){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const float lambertian = max(dot(lightVec, normal), 0.0);

    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * lambertian);
    
    const vec3 halfVec = normalize(normalize(-vertexPosition) + lightVec);

    const float specularFactor = pow(max(dot(halfVec, normal), 0.0), specularExponent);

    const vec3 specular = (FragmentPushConstantData.lightBuffer.lightIntensity * FragmentPushConstantData.materialBuffer.materialSpecular * specularFactor).xyz;

    return ambient + diffuse + specular;
}

vec3 toonShader(in vec3 vertexPosition, in vec3 normal, in vec3 lightVec){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);
    const float lambertian = max(dot(lightVec, normal), 0.0);

    const int levels = 4;
    const float scaleFactor = 1.0 / levels;
    
    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * floor(lambertian * levels) * scaleFactor);

    return ambient + (FragmentPushConstantData.lightBuffer.lightIntensity.xyz * diffuse);
}

vec3 La = vec3(0.3686, 0.3647, 0.3647); // ambient light colour
vec3 Ld = vec3(1.0, 1.0, 1.0); // diffuse light colour
vec3 Ls = vec3(1.0, 1.0, 1.0); // specular light colour

vec3 spotLight(in vec3 vertexPosition, in vec3 normal, in vec3 lightVec){

    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const vec3 spotLightDir = vec3(0.0, 0.0, -1.0);

    const float spotArc = 0.906;
    const float spotDot = dot(spotLightDir, lightVec);

    float spotFactor = (spotDot - spotArc) / (1.0 - spotArc);
    spotFactor = clamp(spotFactor, 0.0, 1.0);

    const float lambertian = max(dot(lightVec, normal), 0.0);

    const vec3 diffuse = Ld * lambertian * spotFactor;

    const vec3 halfVec = normalize(normalize(-vertexPosition) + lightVec);

    const float specularFactor = pow(max(dot(halfVec, normal), 0.0), specularExponent);

    const vec3 specular = Ls * specularFactor * spotFactor;

    return La + diffuse + specular;
}

void main(){
    const vec4 texMap = texture(myTextures[FragmentPushConstantData.diffuseTextureID], inTexCoord);
    const vec3 lightInEyeSpace = (UniformData.viewMatrix * vec4(FragmentPushConstantData.lightBuffer.lightPosition.xyz, 1.0)).xyz;
    const vec3 lightVec = normalize(lightInEyeSpace - inPosition);

    // const vec3 lightInCome = phongLightModel(inPosition, normalize(inNormal),lightVec);

    // const vec3 lightInCome = blindPhongLighModel(inPosition, normalize(inNormal), lightVec);

    // const vec3 lightInCome = toonShader(inPosition, normalize(inNormal), lightVec);

    // const vec3 lightInCome = spotLight(inPosition, normalize(inNormal), lightVec);

    // finalColor = texMap * vec4(lightInCome, 1.0);

    finalColor = texMap * vec4(spotLight(inPosition, normalize(inNormal), lightVec), 1.0);

    finalEntityID = inEntityID;
}