#version 460 core

layout(set = 0, binding = 0) uniform _uniformData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
} UniformData;


layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

const float GridSize = 100.0;
const float GridCellSize = 0.5;
const float GridMinPixelsBetweenCells = 2.0;
const vec4 GridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 GridColorThick = vec4(1.0, 1.0, 1.0, 1.0);

float maxValueVec(vec2 vec){
    return max(vec.x, vec.y);
}

vec2 saturateVec(vec2 vec){
    return clamp(vec, vec2(0.0, 0.0), vec2(1.0, 1.0));
}

float log10(float x){
    return log(x) / log(10.0);
}

float genGrid1(in vec3 position){
    const vec2 dvx = vec2(dFdx(position.x), dFdy(position.x));
    const vec2 dvy = vec2(dFdx(position.z), dFdy(position.z));

    const float lx = length(dvx);
    const float ly = length(dvy);

    vec2 dvxy = vec2(lx, ly);

    dvxy *= 4.0;

    const vec2 tempVal = mod(position.xz, GridCellSize) / dvxy;
    const float levelOfDetailAlpha = maxValueVec(vec2(1.0, 1.0) - abs(saturateVec(tempVal) * 2.0 - vec2(1.0, 1.0)));

    return levelOfDetailAlpha;
}

float genGrid2(in vec3 position){
    const vec2 dvx = vec2(dFdx(position.x), dFdy(position.x));
    const vec2 dvy = vec2(dFdx(position.z), dFdy(position.z));

    const float lx = length(dvx);
    const float ly = length(dvy);

    vec2 dvxy = vec2(lx, ly);

    const float lxy = length(dvxy);

    const float levelOfDetail = max(0.0, log10(lxy - GridMinPixelsBetweenCells / GridCellSize) + 1.0);

    const float gridCellLevelOfDetail = GridCellSize * pow(10.0, floor(levelOfDetail));

    dvxy *= 4.0;

    const vec2 tempVal = mod(position.xz, gridCellLevelOfDetail) / dvxy;
    const float levelOfDetailAlpha = maxValueVec(vec2(1.0, 1.0) - abs(saturateVec(tempVal) * 2.0 - vec2(1.0, 1.0)));

    return levelOfDetailAlpha;
}

vec4 genGrid3(in vec3 position){

    const vec2 dvx = vec2(dFdx(position.x), dFdy(position.x));
    const vec2 dvy = vec2(dFdx(position.z), dFdy(position.z));

    const float lx = length(dvx);
    const float ly = length(dvy);

    vec2 dvxy = vec2(lx, ly);

    const float lxy = length(dvxy);

    const float levelOfDetail = max(0.0, log10(lxy - GridMinPixelsBetweenCells / GridCellSize) + 1.0);

    const float gridCellLevelOfDetail0 = GridCellSize * pow(10.0, floor(levelOfDetail));

    const float gridCellLevelOfDetail1 = gridCellLevelOfDetail0 * 10.0;

    const float gridCellLevelOfDetail2 = gridCellLevelOfDetail1 * 10.0;

    dvxy *= 4.0;

    vec2 tempVal = mod(position.xz, gridCellLevelOfDetail0) / dvxy;
    const float levelOfDetailAlpha0 = maxValueVec(vec2(1.0, 1.0) - abs(saturateVec(tempVal) * 2.0 - vec2(1.0, 1.0)));

    tempVal = mod(position.xz, gridCellLevelOfDetail1) / dvxy;
    const float levelOfDetailAlpha1 = maxValueVec(vec2(1.0, 1.0) - abs(saturateVec(tempVal) * 2.0 - vec2(1.0, 1.0)));

    tempVal = mod(position.xz, gridCellLevelOfDetail2) / dvxy;
    const float levelOfDetailAlpha2 = maxValueVec(vec2(1.0, 1.0) - abs(saturateVec(tempVal) * 2.0 - vec2(1.0, 1.0)));

    const float levelOfDetailFade = fract(levelOfDetail);

    vec4 finalColor;

    if(levelOfDetailAlpha2 > 0.0){
        finalColor = GridColorThick;
        finalColor.a *= levelOfDetailAlpha2;
    }
    else{
        if(levelOfDetailAlpha1 > 0.0){
            finalColor = mix(GridColorThick, GridColorThin, levelOfDetailFade);
            finalColor.a *= levelOfDetailAlpha1;
        }
        else{
            finalColor = GridColorThin;
            finalColor.a *= levelOfDetailAlpha0 * (1.0 - levelOfDetailFade);
        }
    }

    const float fallOff = (1.0 - clamp(length(position.xz - UniformData.cameraPosition.xz) / GridSize, 0.0, 1.0));

    finalColor.a *= fallOff;
    return finalColor;
}

void main() {

    // const float levelOfDetailAlpha = genGrid2(inPosition);

    // vec4 finalColor = GridColorThick;
    // finalColor.a *= levelOfDetailAlpha;

    // outColor = finalColor;

    outColor = genGrid3(inPosition);
}