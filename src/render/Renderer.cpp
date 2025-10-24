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


Renderer::Renderer(Camera* activeCamera, Scene* activeScene, const glm::vec2 viewportSize)
    : m_ImageData(nullptr), m_Width(0), m_Height(0), m_ActiveCamera(activeCamera), m_ActiveScene(activeScene) {
    OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
    m_SceneRenderTimer = std::make_unique<Utils::Timer>();
    m_FrameRenderTimer = std::make_unique<Utils::Timer>();
}

Renderer::RenderingStatus Renderer::Render() {
    if (m_IsRenderingFinished && !m_DumpFramesToDisc) return{
        .FrameIndex = m_FrameIndex,
        .RenderFinished = true,
        .SceneRenderTime = m_SceneRenderTimer->StopAndGetTime(),
        .FrameRenderTime = 0,
    };

    if (m_FrameIndex >= m_Settings.FramesToAccumulate)
    {
        prepareFrame();
        m_IsRenderingFinished = true;
        return {
            .FrameIndex = m_FrameIndex,
            .RenderFinished = true,
            .SceneRenderTime = m_SceneRenderTimer->StopAndGetTime(),
            .FrameRenderTime = 0,
        };
    }
    if (m_FrameIndex == 1) {
        m_AccumulationData.ZeroAll();
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
            m_AccumulationData.AddColor(x, y, color);
        }
    }
#if MT_RENDERING
);
#endif

    if (m_FrameIndex % 10 == 0 || m_FrameIndex == 1) {
        prepareFrame();
    }
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

std::uint32_t* Renderer::GetFinalImageData() const {
    return m_ImageData;
}

void Renderer::OnResize(const uint32_t width, const uint32_t height) {
    m_Width = width;
    m_Height = height;

    delete[] m_ImageData;
    m_ImageData = new std::uint32_t[width * height];

    m_AccumulationData.Resize(width, height);

    ResetFrameIndex();
}

void Renderer::DumpFramesToDisc(const std::string& folder)
{
    m_DumpFolder = folder;
    m_DumpFramesToDisc = true;
}

glm::vec4 Renderer::perPixel(const uint32_t x, const uint32_t y) const {
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + m_Width * y];

    uint32_t seed = x + m_Width * y;
    seed *= m_FrameIndex;

    auto rayColor = this->rayColor(ray, m_Settings.RayBounces, seed);
    return {rayColor, 1.0f};
}

glm::vec3 Renderer::rayColor(const Ray &ray, const int depth, uint32_t &seed) const {
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

    // const glm::vec3 dir = glm::normalize(ray.Direction);
    // const auto a = 0.5f * (dir.y + 1.0f);
    // return 0.4f * ((1.0f - a) * glm::vec3(1.0, 1.0, 1.0) + a * glm::vec3(0.5, 0.7, 1.0));
    return {0.0f, 0.0f, 0.0f};
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

void Renderer::prepareFrame() {
    Image frameBuffer {};
    frameBuffer.Resize(m_AccumulationData.Width, m_AccumulationData.Height);

    auto avgProcessor = AverageFramesProcessor(m_FrameIndex);
    avgProcessor.ProcessImage(m_AccumulationData, frameBuffer);
    if (m_DumpFramesToDisc)
    {
        frameBuffer.WritePng(m_DumpFolder + "/1. AccumulatedFrame_" + std::to_string(m_FrameIndex) + ".png");
    }

    if (m_Settings.BloomEnabled)
    {
        auto bloomFilter = BloomProcessor(m_Settings.BloomThreshold, m_Settings.BloomLevels, m_Settings.BloomRadius,
            m_Settings.BloomSigma, m_Settings.BloomIntensity, m_DumpFramesToDisc, m_DumpFolder);
        bloomFilter.ProcessImage(frameBuffer, frameBuffer);
        if (m_DumpFramesToDisc)
        {
            frameBuffer.WritePng(m_DumpFolder + "/2. BloomOutput_" + std::to_string(m_FrameIndex) + ".png");
        }
    }

    if (m_Settings.HDREnabled)
    {
        auto hdrProcessor = HDRProcessor(m_Settings.Exposure);
        hdrProcessor.ProcessImage(frameBuffer, frameBuffer);
        if (m_DumpFramesToDisc)
        {
            frameBuffer.WritePng(m_DumpFolder + "/3. HDR_" + std::to_string(m_FrameIndex) + ".png");
        }
    }

    if (m_Settings.TonemapEnabled)
    {
        auto toneMapper = TonemapACESProcessor();
        toneMapper.ProcessImage(frameBuffer, frameBuffer);
        if (m_DumpFramesToDisc)
        {
            frameBuffer.WritePng(m_DumpFolder + "/4. ToneMap_" + std::to_string(m_FrameIndex) + ".png");
        }
    }

    if (m_Settings.GammaCorrectionEnabled)
    {
        auto gammaProcessor = GammaCorrectionProcessor(m_Settings.Gamma);
        gammaProcessor.ProcessImage(frameBuffer, frameBuffer);
        if (m_DumpFramesToDisc)
        {
            frameBuffer.WritePng(m_DumpFolder + "/5. GammaCorrection_" + std::to_string(m_FrameIndex) + ".png");
        }
    }
    frameBuffer.ToRGBA8(m_ImageData);
    m_DumpFramesToDisc = false;
}
