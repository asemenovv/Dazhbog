#pragma once
#include "math/Hittable.h"

struct ScatterRays {
    Ray Ray{};
    glm::vec3 Attenuation;
    glm::vec3 Emission;
};

class Material {
public:
    virtual ~Material() = default;

    virtual ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const = 0;
};

class DiffuseMaterial final : public Material
{
public:
    DiffuseMaterial(glm::vec3 Albedo, glm::vec3 EmissionColor, float EmissionPower);

    ScatterRays Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
    glm::vec3 m_EmissionColor;
    float m_EmissionPower;
};

class MetalMaterial final : public Material
{
public:
    explicit MetalMaterial(glm::vec3 Albedo);

    ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
};
