#include "Random.h"

#include "glm/ext/scalar_constants.hpp"

namespace Utils {
    uint32_t Random::PCG_Hash(const uint32_t input) {
        const uint32_t state = input * 747796405u + 2891336453u;
        const uint32_t word = (state >> ((state >> 28u) + 4u) ^ state) + 277803737u;
        return word >> 22u ^ word;
    }

    float Random::RandomFloat(uint32_t &seed) {
        seed = PCG_Hash(seed);
        return static_cast<float>(seed) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    float Random::RandomFloat(uint32_t &seed, const float min, const float max) {
        return min + (max - min) * RandomFloat(seed);
    }

    glm::vec3 Random::InUnitSphere(uint32_t &seed) {
        return glm::normalize(glm::vec3(
            RandomFloat(seed, -1.0f, 1.0f),
            RandomFloat(seed, -1.0f, 1.0f),
            RandomFloat(seed, -1.0f, 1.0f)
        ));
    }

    glm::vec3 Random::RandomInHemisphere(uint32_t &seed, const glm::vec3 &normal) {
        const float u = RandomFloat(seed, 0.0f, 1.0f);
        const float v = RandomFloat(seed, 0.0f, 1.0f);
        const float theta = 2.0f * glm::pi<float>() * u;
        const float phi = acos(1.0f - 2.0f * v);
        const glm::vec3 dir(
            sin(phi) * cos(theta),
            sin(phi) * sin(theta),
            cos(phi)
        );
        return glm::dot(dir, normal) > 0.0f ? dir : -dir;
    }

    uint32_t Random::SeedHash(uint32_t x, uint32_t y, uint32_t s, uint32_t frame) {
        uint32_t h = x * 374761393u + y * 668265263u;
        h ^= (s + 1u) * 2246822519u;
        h ^= frame * 3266489917u;
        h ^= h >> 13;
        h *= 1274126177u;
        h ^= h >> 16;
        return h;
    }
}
