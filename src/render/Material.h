#pragma once
#include "math/Hittable.h"

struct ScatterRays {
    Ray Ray{};
    glm::vec3 Attenuation;
    bool Scattered = true;
    glm::vec3 Emission = { 0.0f, 0.0f, 0.0f };
};

class Material {
public:
    virtual ~Material() = default;

    virtual ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const = 0;
};

class LambertMaterial final : public Material
{
public:
    explicit LambertMaterial(glm::vec3 Albedo);

    ScatterRays Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
};

class MetalMaterial final : public Material
{
public:
    explicit MetalMaterial(glm::vec3 albedo, float fuzziness);

    ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_Albedo;
    float m_Fuzziness;
};

class DiffuseLightMaterial final : public Material
{
public:
    explicit DiffuseLightMaterial(glm::vec3 emissionColor, float emissionPower);

    ScatterRays Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const override;
private:
    glm::vec3 m_EmissionColor;
    float m_EmissionPower;
};

class DielectricMaterial final : public Material
{
public:
    explicit DielectricMaterial(float refractionIndex);

    ScatterRays Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t &randomSeed) const override;

private:
    static double reflectance(double cosine, double refractionIndex);

    float m_RefractionIndex;
};
