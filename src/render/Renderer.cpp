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
            m_ImageData[y * m_Width + x] = this->perPixel(coord);
        }
    }
}

void Renderer::OnResize(const uint32_t width, const uint32_t height) {
    m_Width = width;
    m_Height = height;
    delete[] m_ImageData;
    m_ImageData = new std::uint32_t[width * height];
}

uint32_t Renderer::perPixel(const glm::vec2 coord) {
    const Ray viewRay({coord.x, coord.y, -1.0f});
    const Sphere sphere(0.5f, glm::vec3(0.0f, 0.0f, -2.0f));

    if (sphere.Intersects(viewRay)) {
        return 0xffff00ff;
    }
    return 0xff000000;
}
