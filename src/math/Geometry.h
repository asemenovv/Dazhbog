#pragma once
#include "glm/vec3.hpp"

struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;

    explicit Ray(const glm::vec3 &direction, const glm::vec3 &origin = glm::vec3(0.0f))
        : Origin(origin), Direction(direction) {
    }
};

struct Sphere {
    glm::vec3 Center;
    float Radius;

    explicit Sphere(const float radius, const glm::vec3 &center = glm::vec3(0.0f))
        : Center(center),
          Radius(radius) {
    }

    [[nodiscard]] bool Intersects(const Ray &ray) const {
        const float a = glm::dot(ray.Direction, ray.Direction);
        const float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, Center));
        const float c = glm::dot(ray.Origin, ray.Origin)
            + glm::dot(Center, Center)
            - 2.0f * glm::dot(ray.Origin, Center)
            - Radius * Radius;

        // D = b^2 - 4ac
        const float discriminant = (b * b) - (4 * a * c);
        return discriminant >= 0.0f;
    }
};