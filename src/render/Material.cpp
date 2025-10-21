#include "Material.h"

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/epsilon.hpp"

namespace Utils {
    float Epsilon = 1e-8f;

    static uint32_t PCG_Hash(const uint32_t input) {
        const uint32_t state = input * 747796405u + 2891336453u;
        const uint32_t word = (state >> ((state >> 28u) + 4u) ^ state) + 277803737u;
        return word >> 22u ^ word;
    }

    static float RandomFloat(uint32_t& seed) {
        seed = PCG_Hash(seed);
        return static_cast<float>(seed) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    static float RandomFloat(uint32_t& seed, const double min, const double max) {
        return min + (max - min) * RandomFloat(seed);
    }

    static glm::vec3 InUnitSphere(uint32_t& seed) {
        return glm::normalize(glm::vec3(
            RandomFloat(seed, -1.0f, 1.0f),
            RandomFloat(seed, -1.0f, 1.0f),
            RandomFloat(seed, -1.0f, 1.0f)
        ));
    }

    static glm::vec3 RandomInHemisphere(uint32_t& seed, const glm::vec3& normal) {
        const float u = RandomFloat(seed, 0.0f, 1.0f);
        const float v = RandomFloat(seed, 0.0f, 1.0f);
        const float theta = 2.0f * glm::pi<float>() * u;
        const float phi = acos(1.0f - 2.0f * v);
        const glm::vec3 dir(
            sin(phi) * cos(theta),
            sin(phi) * sin(theta),
            cos(phi)
        );
        return (glm::dot(dir, normal) > 0.0f) ? dir : -dir;
    }
}

DiffuseMaterial::DiffuseMaterial(const glm::vec3 Albedo)
    : m_Albedo(Albedo) {
}

ScatterRays DiffuseMaterial::Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const {
    ScatterRays scattered{};
    glm::vec3 scatterDirection = hitPayload.WorldNormal + Utils::InUnitSphere(randomSeed);
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
    const glm::vec3 fuzzyComponent = m_Fuzziness * Utils::InUnitSphere(randomSeed);
    glm::vec3 reflected = glm::normalize(glm::reflect(ray.Direction, hitPayload.WorldNormal));
    reflected = reflected + fuzzyComponent;

    const auto normalWithOffset = hitPayload.WorldNormal;// * 0.0001f;
    scattered.Ray = Ray(reflected, hitPayload.WorldPosition + normalWithOffset);
    scattered.Attenuation = m_Albedo;
    scattered.Scattered = glm::dot(scattered.Ray.Direction, normalWithOffset) > 0.0f;
    return scattered;
}
