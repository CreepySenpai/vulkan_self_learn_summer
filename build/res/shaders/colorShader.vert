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

layout(location = 0) in vec3 inVertex;

layout(location = 0) out vec3 outColor;
void main(){
    gl_Position = vec4(inVertex, 1.0);
    outColor = inVertex;
}