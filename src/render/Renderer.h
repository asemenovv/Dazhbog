#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Camera.h"
#include "math/Geometry.h"
#include "scene/Scene.h"

class Renderer {
public:
    struct Settings {
        bool Accumulate = true;
    };

    explicit Renderer(Camera* activeCamera, Scene* activeScene, glm::vec2 viewportSize);

    void Render();

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] std::uint32_t* GetFinalImageData() const { return m_ImageData; }

    void ResetFrameIndex() { m_FrameIndex = 1; }

    Settings GetSettings() const { return m_Settings; }
private:
    glm::vec4 perPixel(uint32_t x, uint32_t y) const; // like RayGen shader

    HitPayload traceRay(const Ray& ray) const;

    Settings m_Settings;

    std::uint32_t*  m_ImageData;
    glm::vec4* m_AccumulationData;
    uint32_t m_Width, m_Height;
    uint32_t m_FrameIndex = 1;

    Camera* m_ActiveCamera;
    Scene* m_ActiveScene;
};
