#version 460 core

layout(location = 0) in vec3 inColor;

// Uniform Var
layout(set = 0, binding = 0) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

void main(){
    finalColor = vec4(inColor, 1.0);
}