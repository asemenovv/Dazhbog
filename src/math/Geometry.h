#pragma once
#include "glm/vec3.hpp"

struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;

    explicit Ray(const glm::vec3 &direction, const glm::vec3 &origin = glm::vec3(0.0f))
        : Origin(origin), Direction(glm::normalize(direction)) {
    }

    [[nodiscard]] glm::vec3 PointAt(float t) const { return Origin + t * Direction; }
};

struct SphereIntersections
{
    uint8_t NumberOfIntersections;
    glm::vec3 FirstIntersection;
    glm::vec3 SecondIntersection;

    explicit SphereIntersections(uint8_t numberOfIntersections = 0, const glm::vec3& firstIntersection = glm::vec3(0.0f),
                                 const glm::vec3& secondIntersection = glm::vec3(0.0f))
        : NumberOfIntersections(numberOfIntersections),
          FirstIntersection(firstIntersection),
          SecondIntersection(secondIntersection) {
    }
};

struct Sphere {
    glm::vec3 Center;
    float Radius;

    explicit Sphere(const float radius, const glm::vec3 &center = glm::vec3(0.0f))
        : Center(center),
          Radius(radius) {
    }

    [[nodiscard]] SphereIntersections Intersects(const Ray &ray) const {
        const float a = glm::dot(ray.Direction, ray.Direction);
        const float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, Center));
        const float c = glm::dot(ray.Origin, ray.Origin)
            + glm::dot(Center, Center)
            - 2.0f * glm::dot(ray.Origin, Center)
            - Radius * Radius;

        // D = b^2 - 4ac
        const float discriminant = (b * b) - (4 * a * c);
        if (discriminant < 0) {
            return SphereIntersections(0);
        }
        const float t1 = (-b + sqrt(discriminant)) / (2 * a);
        if (discriminant == 0) {
            return SphereIntersections(1, ray.PointAt(t1));
        }
        const float t2 = (-b - sqrt(discriminant)) / (2 * a);
        if (t1 < t2) {
            return SphereIntersections(2, ray.PointAt(t1), ray.PointAt(t2));
        }
        return SphereIntersections(2, ray.PointAt(t2), ray.PointAt(t1));
    }

    glm::vec3 NormalAtPoint(const glm::vec3 &point) const {
        return glm::normalize(point - Center);
    }
};