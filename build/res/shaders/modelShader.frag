#version 460 core

#extension GL_EXT_buffer_reference : require

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer LightBuffer{
    vec4 lightPosition;
    vec4 ambientColor;
    vec4 diffuseIntensity;
    vec4 lightIntensity;
};

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer MaterialBuffer{
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
};

layout(push_constant) uniform _fragmentPushConstant{
    LightBuffer lightBuffer;
    MaterialBuffer materialBuffer;
} FragmentPushConstantData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Uniform Var
layout(set = 1, binding = 0) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

void main(){
    vec4 texMap = texture(myTexture, inTexCoord);
    
    finalColor = texMap * vec4(1.0, 0.0, 0.0, 1.0);
}