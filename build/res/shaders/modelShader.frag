#version 460 core

#extension GL_EXT_buffer_reference : require

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
} FragmentPushConstantData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Uniform Var
layout(set = 1, binding = 0) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

vec3 phongLightModel(vec3 position, vec3 normal){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const vec3 lightDir = normalize(FragmentPushConstantData.lightBuffer.lightPosition.xyz - position);

    const float lDotN = max(dot(lightDir, normal), 0.0);

    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * lDotN);

    vec3 specular = vec3(0.0, 0.0, 0.0);
    if(lDotN > 0.0){
        const vec3 eyeDir = -position;
        
        const vec3 reflectVec = reflect(-lightDir, normal);

        specular = vec3(FragmentPushConstantData.lightBuffer.lightIntensity * FragmentPushConstantData.materialBuffer.materialSpecular * pow(max(dot(eyeDir, reflectVec), 0.0), 80));
    }

    return ambient + diffuse + specular;
}

void main(){
    const vec4 texMap = texture(myTexture, inTexCoord);

    const vec3 lightInCome = phongLightModel(inPosition, inNormal);

    finalColor = texMap * vec4(lightInCome, 1.0);

}