#include "Material.h"

#include <glm/gtc/epsilon.hpp>

#include "math/ColorUtils.h"
#include "math/Random.h"

LambertMaterial::LambertMaterial(const glm::vec3 Albedo)
    : m_Albedo(Albedo) {
}

ScatterRays LambertMaterial::Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const {
    ScatterRays scattered{};
    glm::vec3 scatterDirection = hitPayload.WorldNormal + Utils::Random::InUnitSphere(randomSeed);
    if (glm::all(glm::epsilonEqual(scatterDirection, glm::vec3(0.0), Utils::Epsilon)))
    {
        scatterDirection = hitPayload.WorldNormal;
    }

    scattered.Ray.Origin = hitPayload.WorldPosition + hitPayload.WorldNormal * 0.0001f;
    scattered.Ray.Direction = scatterDirection;
    scattered.Attenuation = m_Albedo;
    scattered.Scattered = true;
    return scattered;
}

MetalMaterial::MetalMaterial(const glm::vec3 albedo, const float fuzziness)
    : m_Albedo(albedo), m_Fuzziness(fuzziness < 1.0f ? fuzziness : 1.0f)
{
}

ScatterRays MetalMaterial::Scatter(const Ray& ray, const HitPayload& hitPayload, uint32_t& randomSeed) const
{
    ScatterRays scattered{};
    const glm::vec3 fuzzyComponent = m_Fuzziness * Utils::Random::InUnitSphere(randomSeed);
    glm::vec3 reflected = glm::normalize(glm::reflect(ray.Direction, hitPayload.WorldNormal));
    reflected = reflected + fuzzyComponent;

    const auto normalWithOffset = hitPayload.WorldNormal;// * 0.0001f;
    scattered.Ray = Ray(hitPayload.WorldPosition + normalWithOffset, reflected);
    scattered.Attenuation = m_Albedo;
    scattered.Scattered = glm::dot(scattered.Ray.Direction, normalWithOffset) > 0.0f;
    return scattered;
}

DiffuseLightMaterial::DiffuseLightMaterial(const glm::vec3 emissionColor, const float emissionPower)
    : m_EmissionColor(ColorUtils::SRGBToLinear(emissionColor)), m_EmissionPower(emissionPower) {
}

ScatterRays DiffuseLightMaterial::Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t &randomSeed) const {
    return {
        .Scattered = false,
        .Emission = m_EmissionPower * m_EmissionColor
    };
}

DielectricMaterial::DielectricMaterial(float refractionIndex): m_RefractionIndex(refractionIndex) {
}

ScatterRays DielectricMaterial::Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t &randomSeed) const {
    const float ri = hitPayload.FrontFace  ? (1.0 / m_RefractionIndex) : m_RefractionIndex;
    const glm::vec3 direction = glm::normalize(ray.Direction);

    double cosTheta = std::fmin(glm::dot(-direction, hitPayload.WorldNormal), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = ri * sinTheta > 1.0;
    ScatterRays scattered{};

    if (cannotRefract || reflectance(cosTheta, ri) > Utils::Random::RandomFloat(randomSeed))
    {
        glm::vec3 reflect = glm::reflect(direction, hitPayload.WorldNormal);
        scattered.Ray = Ray(hitPayload.WorldPosition, reflect);
    }
    else
    {
        glm::vec3 refract = glm::refract(direction, hitPayload.WorldNormal, ri);
        scattered.Ray = Ray(hitPayload.WorldPosition, refract);
    }

    scattered.Emission = {0.0f, 0.0f, 0.0f};
    scattered.Attenuation = {1.0f, 1.0f, 1.0f};
    scattered.Scattered = true;
    return scattered;
}

double DielectricMaterial::reflectance(const double cosine, const double refractionIndex) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine),5);
}
