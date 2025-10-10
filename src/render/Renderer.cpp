#include "Renderer.h"

#include "../math/Geometry.h"

void Renderer::Render() {
    for (int x = 0; x < m_Width; x++) {
        for (int y = 0; y < m_Height; y++) {
            glm::vec2 coord = {
                static_cast<float>(x) / static_cast<float>(m_Width),
                static_cast<float>(y) / static_cast<float>(m_Height)
            };
            coord = coord * 2.0f - 1.0f;
            m_ImageData[y * m_Width + x] = vec4ToRGBA8(this->perPixel(coord));
        }
    }
}

void Renderer::OnResize(const uint32_t width, const uint32_t height) {
    m_Width = width;
    m_Height = height;
    delete[] m_ImageData;
    m_ImageData = new std::uint32_t[width * height];
}

glm::vec4 SPHERE_COLOR = glm::vec4{0.0, 0.0, 1.0, 1.0};
glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
float AMBIENT = 0.1f;

glm::vec4 Renderer::perPixel(const glm::vec2 coord) {
    const Ray viewRay({coord.x, coord.y, -1.0f});
    const Sphere sphere(0.5f, glm::vec3(0.0f, 0.0f, -2.0f));

    auto sphereIntersections = sphere.Intersects(viewRay);
    if (sphereIntersections.NumberOfIntersections > 0) {
        const auto normal = sphere.NormalAtPoint(sphereIntersections.FirstIntersection);
        float diffuseIntensity = std::max(glm::dot(normal, -LIGHT_DIRECTION), 0.0f);
        auto ambient = AMBIENT * SPHERE_COLOR;
        auto diffuse = diffuseIntensity * SPHERE_COLOR;
        return ambient + diffuse;
    }
    return {0.0, 0.0, 0.0, 1.0};
}

uint32_t Renderer::vec4ToRGBA8(glm::vec4 color) {
    const uint8_t r = static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
    const uint8_t g = static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f);
    const uint8_t b = static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f + 0.5f);
    const uint8_t a = static_cast<uint8_t>(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f + 0.5f);
    return static_cast<uint32_t>(a) << 24
    | static_cast<uint32_t>(b) << 16
    | static_cast<uint32_t>(g) << 8
    | static_cast<uint32_t>(r);
}
