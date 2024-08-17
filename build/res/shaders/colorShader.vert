#version 460 core

// struct Vertex{
//     vec3 Position;
//     vec3 Normal;
//     vec2 TexCoord;
// };

// layout(set = 0, binding = 0) uniform TransFormData{
//     mat4 modelMatrix;
//     mat4 viewMatrix;
//     mat4 projectionMatrix;
// } MVPMatrix;

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;


void main(){
    // const vec3 positions[3] = vec3[3](
	// 	vec3(1.f,1.f, 0.0f),
	// 	vec3(-1.f,1.f, 0.0f),
	// 	vec3(0.f,-1.f, 0.0f)
	// );

	// //const array of colors for the triangle
	// const vec3 colors[3] = vec3[3](
	// 	vec3(1.0f, 0.0f, 0.0f), //red
	// 	vec3(0.0f, 1.0f, 0.0f), //green
	// 	vec3(00.f, 0.0f, 1.0f)  //blue
	// );

    // gl_Position = vec4(positions[gl_VertexIndex], 1.0);

	// outColor = colors[gl_VertexIndex];

    gl_Position = vec4(inVertex, 1.0);

    outColor = inNormal;
}