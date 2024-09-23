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
layout(location = 3) in vec3 inCameraPosition;

// Uniform Var
layout(set = 1, binding = 0) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

vec3 phongLightModel(in vec3 vertexPosition, in vec3 normal, in vec3 cameraPosition){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const vec3 lightDir = normalize(FragmentPushConstantData.lightBuffer.lightPosition.xyz - vertexPosition);

    const float lambertian = max(dot(lightDir, normal), 0.0);

    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * lambertian);

    vec3 specular = vec3(0.0, 0.0, 0.0);

    if(lambertian > 0.0){
        const vec3 viewVec = normalize(cameraPosition - vertexPosition);
        
        const vec3 reflectVec = reflect(-lightDir, normal);

        specular = vec3(FragmentPushConstantData.lightBuffer.lightIntensity * FragmentPushConstantData.materialBuffer.materialSpecular * pow(max(dot(viewVec, reflectVec), 0.0), 80));
    }

    return ambient + diffuse + specular;
}

vec3 blindPhongLighModel(in vec3 vertexPosition, in vec3 normal, in vec3 cameraPosition){
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);

    const vec3 lightDir = normalize(FragmentPushConstantData.lightBuffer.lightPosition.xyz - vertexPosition);

    const float lambertian = max(dot(lightDir, normal), 0.0);

    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * lambertian);

    vec3 specular = vec3(0.0, 0.0, 0.0);

    if(lambertian > 0.0){
        const vec3 viewVec = normalize(cameraPosition - vertexPosition);
        const vec3 halfVec = normalize(lightDir + viewVec);

        specular = vec3(FragmentPushConstantData.lightBuffer.lightIntensity * FragmentPushConstantData.materialBuffer.materialSpecular * pow(max(dot(halfVec, normal), 0.0), 80));
    }

    return ambient + diffuse + specular;
}

vec3 toonShader(in vec3 vertexPosition, in vec3 normal, in vec3 cameraPosition){
    const vec3 lightDir = normalize(FragmentPushConstantData.lightBuffer.lightPosition.xyz - vertexPosition);
    const vec3 ambient = vec3(FragmentPushConstantData.lightBuffer.lightAmbientColor * FragmentPushConstantData.materialBuffer.materialAmbient);
    const float lambertian = max(dot(lightDir, normal), 0.0);

    const int levels = 4;
    const float scaleFactor = 1.0 / levels;
    
    const vec3 diffuse = vec3(FragmentPushConstantData.lightBuffer.lightDiffuseIntensity * FragmentPushConstantData.materialBuffer.materialDiffuse * floor(lambertian * levels) * scaleFactor);

    return ambient + (FragmentPushConstantData.lightBuffer.lightIntensity.xyz * diffuse);
}

void main(){
    const vec4 texMap = texture(myTexture, inTexCoord);

    // const vec3 lightInCome = phongLightModel(inPosition, normalize(inNormal), inCameraPosition);

    // const vec3 lightInCome = blindPhongLighModel(inPosition, normalize(inNormal), inCameraPosition);

    const vec3 lightInCome = toonShader(inPosition, normalize(inNormal), inCameraPosition);

    finalColor = texMap * vec4(lightInCome, 1.0);

}