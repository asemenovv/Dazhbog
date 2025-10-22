#pragma once

#include <glm/vec3.hpp>


namespace ColorUtils {
    inline float SRGBToLinear(const float u) {
        return (u <= 0.04045f) ? (u / 12.92f)
                               : std::pow((u + 0.055f) / 1.055f, 2.4f);
    }

    inline glm::vec3 SRGBToLinear(const glm::vec3& c) {
        return { SRGBToLinear(c.r), SRGBToLinear(c.g), SRGBToLinear(c.b) };
    }
};
