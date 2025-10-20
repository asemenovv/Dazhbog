#include "Material.h"

#include "glm/ext/scalar_constants.hpp"

namespace Utils {
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

DiffuseMaterial::DiffuseMaterial(const glm::vec3 Albedo, const glm::vec3 EmissionColor, const float EmissionPower)
    : m_Albedo(Albedo), m_EmissionColor(EmissionColor), m_EmissionPower(EmissionPower) {
}

ScatterRays DiffuseMaterial::Scatter(const Ray &ray, const HitPayload &hitPayload, uint32_t& randomSeed) const {
    ScatterRays scattered{};
    scattered.Ray.Origin = hitPayload.WorldPosition + hitPayload.WorldNormal * 0.0001f;
    scattered.Ray.Direction = hitPayload.WorldNormal + Utils::InUnitSphere(randomSeed);
    scattered.Attenuation = m_Albedo;
    scattered.Emission = m_EmissionPower * m_EmissionColor;
    return scattered;
}
