#pragma once

#include <cstdint>
#include <QTimer>
#include <glm/glm.hpp>

#include "Camera.h"
#include "ImagePostProcessors.h"
#include "math/Geometry.h"
#include "scene/Scene.h"
#include "utils/Timer.h"

#define MT_RENDERING 1

class Renderer {
public:
    struct Settings
    {
        bool Accumulate = true;
        uint32_t FramesToAccumulate = 300;
        float Gamma = 2.2f;
        float Exposure = 0.0f;
        uint32_t RayBounces = 5;
        float BloomThreshold = 1.0f;
        uint32_t BloomLevels;
    };
    struct RenderingStatus
    {
        uint32_t FrameIndex = 0;
        bool RenderFinished = false;
        uint64_t SceneRenderTime = 0;
        uint64_t FrameRenderTime = 0;
    };

    explicit Renderer(Camera* activeCamera, Scene* activeScene, glm::vec2 viewportSize);

    RenderingStatus Render();

    [[nodiscard]] std::uint32_t* GetFinalImageData() const;

    void OnResize(uint32_t width, uint32_t height);

    void ResetFrameIndex() {
        m_IsRenderingFinished = false;
        m_FrameIndex = 1;
    }

    Settings& GetSettings() { return m_Settings; }
private:
    glm::vec4 perPixel(uint32_t x, uint32_t y) const; // like RayGen shader

    glm::vec3 rayColor(const Ray& ray, int depth, uint32_t &seed) const;

    HitPayload traceRay(const Ray& ray) const;

    void prepareFrame(bool applyPostProcessors);

    Settings m_Settings;

    std::uint32_t*  m_ImageData;
    Image m_AccumulationData;
    uint32_t m_Width, m_Height;
    uint32_t m_FrameIndex = 1;

    Camera* m_ActiveCamera;
    Scene* m_ActiveScene;

    std::unique_ptr<Utils::Timer> m_SceneRenderTimer;
    std::unique_ptr<Utils::Timer> m_FrameRenderTimer;

    bool m_IsRenderingFinished = false;
};
