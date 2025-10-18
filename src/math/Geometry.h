#pragma once

#include <glm/glm.hpp>

#include "Hittable.h"

class Sphere final : public Hittable {
public:
    explicit Sphere(float radius, uint32_t materialIndex = 0, const glm::vec3 &center = glm::vec3(0.0f));

    ~Sphere() override = default;

    HitPayload Hit(const Ray& ray, Interval tBoundaries) const override;

    [[nodiscard]] glm::vec3 NormalAtPoint(const glm::vec3 &point) const;

    uint32_t GetMaterialIndex() const override { return m_MaterialIndex; }

    [[nodiscard]] glm::vec4 GetCenter() const { return {m_Center, 1.0f}; }

    void MoveTo(const glm::vec3 &point);

private:
    glm::vec3 m_Center;
    float m_Radius;
    uint32_t m_MaterialIndex;
};


class Triangle final : public Hittable {
public:
    explicit Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, uint32_t materialIndex);

    ~Triangle() override = default;

    HitPayload Hit(const Ray& ray, Interval tBoundaries) const override;

    [[nodiscard]] glm::vec3 Normal() const;

    uint32_t GetMaterialIndex() const override;

private:
    glm::vec3 m_A;
    glm::vec3 m_B;
    glm::vec3 m_C;
    uint32_t m_MaterialIndex;
};