#pragma once

#include <glm/glm.hpp>

namespace Creepy {

    struct UniformData{
        glm::mat4 worldMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 projectionMatrix{1.0f};
        glm::vec4 cameraPosition{1.0f};
    };
    
}