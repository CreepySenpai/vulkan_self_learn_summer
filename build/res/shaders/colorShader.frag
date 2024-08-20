#version 460 core

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

// Uniform Var
layout(set = 0, binding = 1) uniform sampler2D myTexture;

layout(location = 0) out vec4 finalColor;

void main(){
    vec4 texMap = texture(myTexture, inTexCoord * 2.0);
    // finalColor = vec4(inColor, 1.0) * texMap;
    finalColor = texMap;
}