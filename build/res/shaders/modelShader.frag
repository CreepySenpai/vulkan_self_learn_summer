#version 460 core

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

// Uniform Var
layout(set = 1, binding = 0) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

void main(){
    vec4 texMap = texture(myTexture, inTexCoord);
    finalColor = texMap;
}