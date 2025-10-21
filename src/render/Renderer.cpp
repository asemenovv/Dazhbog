#include "Renderer.h"

#include "scene/Scene.h"
#include "wallnut/Random.h"

#if MT_RENDERING
#ifdef emit
#  undef emit
#endif
#include <tbb/parallel_for.h>
#include <oneapi/tbb/global_control.h>
#endif

#include "utils/Timer.h"

namespace Utils {
    static double LinearToGamma(const double linearComponent)
    {
        if (linearComponent > 0)
            return std::sqrt(linearComponent);
        return 0;
    }

    static uint32_t Vec4ToRGBA8(const glm::vec4 color) {
        const auto r = static_cast<uint8_t>(LinearToGamma(color.r) * 255.0f);
        const auto g = static_cast<uint8_t>(LinearToGamma(color.g) * 255.0f);
        const auto b = static_cast<uint8_t>(LinearToGamma(color.b) * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);
        return a << 24 | b << 16 | g << 8 | r;
    }
}

Renderer::Renderer(Camera* activeCamera, Scene* activeScene, const glm::vec2 viewportSize)
    : m_ImageData(nullptr), m_Width(0), m_Height(0), m_ActiveCamera(activeCamera), m_ActiveScene(activeScene) {
    OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
    m_SceneRenderTimer = std::make_unique<Utils::Timer>();
    m_FrameRenderTimer = std::make_unique<Utils::Timer>();
}

Renderer::RenderingStatus Renderer::Render() {
    if (m_FrameIndex >= m_Settings.FramesToAccumulate)
    {
        return {
            .FrameIndex = m_FrameIndex,
            .RenderFinished = true,
            .SceneRenderTime = m_SceneRenderTimer->StopAndGetTime(),
            .FrameRenderTime = 0,
        };
    }
    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(glm::vec4));
        m_SceneRenderTimer->Start();
    }
    m_FrameRenderTimer->Start();

#if MT_RENDERING
    tbb::global_control limit(tbb::global_control::max_allowed_parallelism, 4);
    tbb::parallel_for<int>(0, m_Height, 1, [this](const int y) {
#else
    for (int y = 0; y < m_Height; y++) {
#endif
        for (int x = 0; x < m_Width; x++)
        {
            const glm::vec4 color = perPixel(x, y);
            m_AccumulationData[x + y * m_Width] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_Width];
            accumulatedColor /= static_cast<float>(m_FrameIndex);

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0), glm::vec4(1.0f));
            m_ImageData[y * m_Width + x] = Utils::Vec4ToRGBA8(accumulatedColor);
        }
    }
#if MT_RENDERING
);
#endif

    if (m_Settings.Accumulate) {
        m_FrameIndex++;
    } else {
        m_FrameIndex = 1;
    }
    return {
        .FrameIndex = m_FrameIndex,
        .RenderFinished = false,
        .SceneRenderTime = 0,
        .FrameRenderTime = m_FrameRenderTimer->StopAndGetTime(),
    };
}

void Renderer::OnResize(const uint32_t width, const uint32_t height) {
    m_Width = width;
    m_Height = height;

    delete[] m_ImageData;
    m_ImageData = new std::uint32_t[width * height];

    delete[] m_AccumulationData;
    m_AccumulationData = new glm::vec4[width * height];

    ResetFrameIndex();
}

glm::vec4 Renderer::perPixel(const uint32_t x, const uint32_t y) const {
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + m_Width * y];

    uint32_t seed = x + m_Width * y;
    seed *= m_FrameIndex;

    constexpr int BOUNCES = 5;
    auto rayColor = this->rayColor(ray, BOUNCES, seed);
    return {rayColor, 1.0f};
}

glm::vec3 Renderer::rayColor(const Ray &ray, int depth, uint32_t &seed) const {
    if (depth <= 0)
        return glm::vec3(0.0f, 0.0f, 0.0f);

    if (const HitPayload hitPayload = traceRay(ray); hitPayload.DidCollide) {
        const Hittable* hittable = m_ActiveScene->GetHittableObjects()[hitPayload.ObjectIndex].get();
        const Material* material = m_ActiveScene->GetMaterials()[hittable->GetMaterialIndex()].get();

        ScatterRays scatterRays = material->Scatter(ray, hitPayload, seed);
        if (scatterRays.Scattered) {
            return scatterRays.Attenuation * rayColor(scatterRays.Ray, depth-1, seed);
        }
        return scatterRays.Emission;
    }

    const glm::vec3 dir = glm::normalize(ray.Direction);
    const auto a = 0.5f * (dir.y + 1.0f);
    return 0.1f * ((1.0f - a) * glm::vec3(1.0, 1.0, 1.0) + a * glm::vec3(0.5, 0.7, 1.0));
    // return {0.0f, 0.0f, 0.0f};
}

HitPayload Renderer::traceRay(const Ray& ray) const {
    float closestSoFar = std::numeric_limits<float>::max();
    HitPayload nearestHitPayload = {.DidCollide = false};
    for (int i = 0; i < m_ActiveScene->GetHittableObjects().size(); i++) {
        const Hittable* hittable = m_ActiveScene->GetHittableObjects()[i].get();
        if (const HitPayload payload = hittable->Hit(ray, Interval(0.0f, closestSoFar)); payload.DidCollide) {
            nearestHitPayload = payload;
            nearestHitPayload.ObjectIndex = i;
            closestSoFar = payload.HitDistance;
        }
    }
    return nearestHitPayload;
}
