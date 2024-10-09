#version 460 core

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

const float GridCellSize = 0.025;
const vec4 GridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 GridColorThick = vec4(1.0, 0.0, 0.0, 1.0);

void main() {
    const float levelOfDetailAlpha = mod(inPosition.z, GridCellSize);

    vec4 finalColor = GridColorThick;
    finalColor.a = 0.5;

    outColor = finalColor;
}