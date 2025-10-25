#pragma once

#include <limits>
#include <glm/glm.hpp>

namespace Utils {
    inline float Epsilon = 1e-8f;

    class Random {
    public:
        Random() = delete;
        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;
        Random& operator=(Random&&) = delete;

        static uint32_t PCG_Hash(uint32_t input);

        static float RandomFloat(uint32_t& seed);

        static float RandomFloat(uint32_t& seed, float min, float max);

        static glm::vec3 InUnitSphere(uint32_t& seed);

        static glm::vec3 RandomInHemisphere(uint32_t& seed, const glm::vec3& normal);

        static uint32_t SeedHash(uint32_t x, uint32_t y, uint32_t s, uint32_t frame);
    };
}
