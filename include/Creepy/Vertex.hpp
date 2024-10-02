#pragma once

#include <glm/glm.hpp>

namespace Creepy{
    
    struct VertexInterLeave{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

    struct VertexSeparate{
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec2> TexCoords;
        // uint32_t EntityID;
    };
}
