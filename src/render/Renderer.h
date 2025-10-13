#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Camera.h"
#include "math/Geometry.h"

class Renderer {
public:
    explicit Renderer(glm::vec2 viewportSize);

    void Render(const Camera* camera);

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] std::uint32_t* GetFinalImageData() const { return m_ImageData; }
private:
    glm::vec4 traceRay(const Ray& viewRay);

    std::uint32_t*  m_ImageData;
    uint32_t m_Width, m_Height;
};
