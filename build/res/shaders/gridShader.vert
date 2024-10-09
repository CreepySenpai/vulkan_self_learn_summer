#version 460 core

layout(set = 0, binding = 0) uniform _uniformData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;

const vec3 gridVertices[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0),
    vec3(1.0, 0.0, -1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(-1.0, 0.0, 1.0)
);

const uint gridIndices[6] = uint[6](0, 2, 1, 2, 0, 3);

layout(location = 0) out vec3 outPosition;  // Position In World Space

void main() {
    const uint currentVertex = gridIndices[gl_VertexIndex];

    vec3 currentVertexPosition = gridVertices[currentVertex];
    
    // Offset By Camera Position
    currentVertexPosition.x += UniformData.cameraPosition.x;
    currentVertexPosition.z += UniformData.cameraPosition.z;

    const vec4 currentVertexForward = vec4(currentVertexPosition, 1.0);

    gl_Position = UniformData.projectionMatrix * UniformData.viewMatrix * currentVertexForward;

    outPosition = currentVertexPosition;
}