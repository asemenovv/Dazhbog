#pragma once
#include "glm/glm.hpp"

struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;

    Ray(): Origin(0, 0, 0), Direction(0, 0, -1) {
    }

    explicit Ray(const glm::vec3 &direction, const glm::vec3 &origin = glm::vec3(0.0f))
        : Origin(origin), Direction(glm::normalize(direction)) {
    }

    [[nodiscard]] glm::vec3 PointAt(const float t) const { return Origin + t * Direction; }
};

struct HitPayload
{
    bool IsHit = false;
    float HitDistance = 0.0;
    glm::vec3 WorldPosition{0.0};
    glm::vec3 WorldNormal{0.0};

    uint32_t ObjectIndex = 0;
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual HitPayload Hit(const Ray& ray, float tMin, float tMax) const = 0;
};
