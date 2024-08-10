#version 460

struct Vertex{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

layout(set = 0, binding = 0) uniform TransFormData{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} MVPMatrix;

void main(){
    
}