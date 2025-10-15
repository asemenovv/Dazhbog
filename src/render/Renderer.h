#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Camera.h"
#include "math/Geometry.h"
#include "scene/Scene.h"

class Renderer {
public:
    explicit Renderer(Camera* activeCamera, Scene* activeScene, glm::vec2 viewportSize);

    void Render();

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] std::uint32_t* GetFinalImageData() const { return m_ImageData; }
private:
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        uint32_t ObjectIndex;
    };

    glm::vec4 perPixel(); // like RayGen shader

    HitPayload traceRay(const Ray& viewRay);
    HitPayload closestHit(const Ray& ray, float hitDistance, uint32_t objectIndex); // like ClosestHit shader
    HitPayload missHit(const Ray& ray); //like Miss shader

    std::uint32_t*  m_ImageData;
    uint32_t m_Width, m_Height;

    Camera* m_ActiveCamera;
    Scene* m_ActiveScene;
};
