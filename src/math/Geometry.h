#pragma once

#include <glm/glm.hpp>

#include "render/Material.h"

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

struct ModelIntersections
{
    uint8_t NumberOfIntersections;
    glm::vec3 FirstIntersection;
    glm::vec3 SecondIntersection;
    float DistanceToNearest;

    explicit ModelIntersections(uint8_t numberOfIntersections = 0, const glm::vec3& firstIntersection = glm::vec3(0.0f),
                                 const glm::vec3& secondIntersection = glm::vec3(0.0f), float distanceToNearest = 0.0f)
        : NumberOfIntersections(numberOfIntersections), FirstIntersection(firstIntersection),
        SecondIntersection(secondIntersection), DistanceToNearest(distanceToNearest)
    {
    }
};

class Sphere {
public:
    explicit Sphere(float radius, uint32_t materialIndex = 0, const glm::vec3 &center = glm::vec3(0.0f));

    [[nodiscard]] ModelIntersections Hit(const Ray &ray) const;

    [[nodiscard]] glm::vec3 NormalAtPoint(const glm::vec3 &point) const;

    uint32_t GetMaterialIndex() const { return m_MaterialIndex; }

    [[nodiscard]] glm::vec4 GetCenter() const { return {m_Center, 1.0f}; };

    void MoveTo(const glm::vec3 &point);
private:
    glm::vec3 m_Center;
    float m_Radius;
    uint32_t m_MaterialIndex;
};