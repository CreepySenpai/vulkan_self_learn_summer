#version 460 core


layout(set = 1, binding = 0) uniform samplerCube cubeMap;

layout(location = 0) in vec3 inTexCoord;

layout(location = 0) out vec4 outColor;

vec4 gammaCorrection(in vec4 currentFragColor){
    const vec3 correctNess = pow(currentFragColor.rgb, vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    return vec4(correctNess, 1.0);
}

void main(){
    outColor = texture(cubeMap, inTexCoord);

    outColor = gammaCorrection(outColor);
}