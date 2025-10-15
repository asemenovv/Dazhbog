#pragma once

#include <glm/glm.hpp>

struct Material {
    glm::vec3 Albedo = glm::vec3(1.0f);
    float Roughness = 1.0f;
    float Metalness = 0.0f;
};
