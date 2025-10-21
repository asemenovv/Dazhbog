#pragma once
#include "math/Hittable.h"

struct ScatterRays {
    Ray Ray{};
    glm::vec3 Attenuation;
    bool Scattered = false;
};

class Material {
public:
    virtual ~Material() = default;

    virtual ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const = 0;
};

class DiffuseMaterial final : public Material
{
public:
    DiffuseMaterial(glm::vec3 Albedo);

    ScatterRays Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
};

class MetalMaterial final : public Material
{
public:
    explicit MetalMaterial(glm::vec3 Albedo);

    ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
};
