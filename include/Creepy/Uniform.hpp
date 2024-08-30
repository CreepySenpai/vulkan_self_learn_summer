#pragma once

#include <glm/glm.hpp>

namespace Creepy {

    struct TransformData{
        glm::mat4 worldMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 projectionMatrix{1.0f};
        glm::vec4 cameraPosition{1.0f};
    };

    struct LightData{
        glm::vec4 lightPosition{0.0f};
        glm::vec4 ambientColor{1.0f};
        glm::vec4 lightIntensity{0.0f};
    };
}