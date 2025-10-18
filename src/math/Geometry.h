#pragma once

#include <glm/glm.hpp>

#include "Hittable.h"

class Sphere final : public Hittable {
public:
    explicit Sphere(float radius, uint32_t materialIndex = 0, const glm::vec3 &center = glm::vec3(0.0f));

    ~Sphere() override = default;

    HitPayload Hit(const Ray& ray, float tMin, float tMax) const override;

    [[nodiscard]] glm::vec3 NormalAtPoint(const glm::vec3 &point) const;

    uint32_t GetMaterialIndex() const { return m_MaterialIndex; }

    [[nodiscard]] glm::vec4 GetCenter() const { return {m_Center, 1.0f}; }

    void MoveTo(const glm::vec3 &point);

private:
    glm::vec3 m_Center;
    float m_Radius;
    uint32_t m_MaterialIndex;
};