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
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        uint32_t ObjectIndex;
    };

    glm::vec4 perPixel(uint32_t x, uint32_t y); // like RayGen shader

    HitPayload traceRay(const Ray& ray);
    HitPayload closestHit(const Ray& ray, float hitDistance, uint32_t objectIndex) const; // like ClosestHit shader
    HitPayload missHit(const Ray& ray); //like Miss shader

    Settings m_Settings;

    std::uint32_t*  m_ImageData;
    glm::vec4* m_AccumulationData;
    uint32_t m_Width, m_Height;
    uint32_t m_FrameIndex = 1;

    Camera* m_ActiveCamera;
    Scene* m_ActiveScene;
};
