#version 460 core

layout(location = 0) in vec3 inColor;

layout(location = 0) out vec4 finalColor;

void main(){
    finalColor = vec4(inColor, 1.0);
}