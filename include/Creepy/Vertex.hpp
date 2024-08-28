#pragma once

#include <glm/glm.hpp>

namespace Creepy{
    
    struct Vertex{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::mat4 Transform;
    };
}
