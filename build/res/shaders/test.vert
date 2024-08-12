#version 460 core

layout(location = 0) in vec3 sus;


void main(){
    gl_Position = vec4(sus, 1.0);
}