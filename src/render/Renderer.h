#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer() = default;

    void Render();

    void OnResize(uint32_t width, uint32_t height);

    std::uint32_t* GetFinalImageData() const { return m_ImageData; }
private:
    glm::vec4 perPixel(glm::vec2 coord);
    uint32_t vec4ToRGBA8(glm::vec4 color);

    std::uint32_t*  m_ImageData;
    uint32_t m_Width, m_Height;
};
