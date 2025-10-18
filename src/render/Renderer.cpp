#include "Renderer.h"

#include "math/Geometry.h"
#include "scene/Scene.h"
#include "wallnut/Random.h"

#include <tbb/parallel_for.h>

namespace Utils {
    static uint32_t Vec4ToRGBA8(const glm::vec4 color) {
        const auto r = static_cast<uint8_t>(color.r * 255.0f);
        const auto g = static_cast<uint8_t>(color.g * 255.0f);
        const auto b = static_cast<uint8_t>(color.b * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);
        return a << 24 | b << 16 | g << 8 | r;
    }

    static uint32_t PCG_Hash(const uint32_t input) {
        const uint32_t state = input * 747796405u + 2891336453u;
        const uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) + 277803737u;
        return (word >> 22u) ^ word;
    }

    static float RandomFloat(uint32_t& seed) {
        seed = PCG_Hash(seed);
        return static_cast<float>(seed) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    static glm::vec3 InUnitSphere(uint32_t& seed) {
        return glm::normalize(glm::vec3(
            RandomFloat(seed) * 2.0 - 1.0,
            RandomFloat(seed) * 2.0 - 1.0,
            RandomFloat(seed) * 2.0 - 1.0
        ));
    }
}

Renderer::Renderer(Camera* activeCamera, Scene* activeScene, const glm::vec2 viewportSize)
    : m_ImageData(nullptr), m_Width(0), m_Height(0), m_ActiveCamera(activeCamera), m_ActiveScene(activeScene) {
    OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
}

void Renderer::Render() {
    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(glm::vec4));
    }

    tbb::parallel_for<int>(0, m_Height, 1, [this](int y) {
        for (int x = 0; x < m_Width; x++)
        {
            const glm::vec4 color = perPixel(x, y);
            m_AccumulationData[x + y * m_Width] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_Width];
            accumulatedColor /= static_cast<float>(m_FrameIndex);

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0), glm::vec4(1.0f));
            m_ImageData[y * m_Width + x] = Utils::Vec4ToRGBA8(accumulatedColor);
        }
    });

    if (m_Settings.Accumulate) {
        m_FrameIndex++;
    } else {
        m_FrameIndex = 1;
    }
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

glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
glm::vec3 SKY_COLOR(0.6f, 0.7f, 0.9f);
float SKY_BRIGHTNESS = 0.2f;
float AMBIENT = 0.1f;

glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + m_Width * y];

    glm::vec3 rayColor(1.0f);
    glm::vec3 brightnessScore(0.0f);

    uint32_t seed = x + m_Width * y;
    seed *= m_FrameIndex;

    constexpr int BOUNCES = 5;
    for (int i = 0; i < BOUNCES; i++) {
        seed += i;

        const HitPayload payload = traceRay(ray);
        if (payload.HitDistance < 0.0f) {
            brightnessScore += SKY_BRIGHTNESS * SKY_COLOR * rayColor;
            break;
        }

        Sphere* sphere = m_ActiveScene->GetSpheres()[payload.ObjectIndex].get();
        Material* material = m_ActiveScene->GetMaterials()[sphere->GetMaterialIndex()].get();
        // const float diffuseIntensity = std::max(glm::dot(payload.WorldNormal, -LIGHT_DIRECTION), 0.0f);
        // const auto diffuse = diffuseIntensity * material->Albedo;
        // const auto ambient = AMBIENT * material->Albedo;
        // const auto sphereColor = diffuse + ambient;
        // light += contribution * material->Albedo;
        brightnessScore += material->GetEmission() * rayColor;
        rayColor *= material->Albedo;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        // ray.Direction = glm::reflect(
            // ray.Direction,
            // payload.WorldNormal + material->Roughness * Walnut::Random::Vec3(-0.5f, 0.5f)
        // );
        ray.Direction = glm::normalize(Utils::InUnitSphere(seed) + payload.WorldNormal);
    }
    return {brightnessScore, 1.0f};
}

Renderer::HitPayload Renderer::traceRay(const Ray& ray)
{
    int closestSphereIndex = -1;
    float hitDistance = std::numeric_limits<float>::max();
    for (int i = 0; i < m_ActiveScene->GetSpheres().size(); i++) {
        const Sphere* sphere = m_ActiveScene->GetSpheres()[i].get();
        const ModelIntersections intersects = sphere->Hit(ray);
        if (intersects.NumberOfIntersections == 0)
            continue;
        if (intersects.DistanceToNearest > 0.0f && intersects.DistanceToNearest < hitDistance) {
            closestSphereIndex = i;
            hitDistance = intersects.DistanceToNearest;
        }
    }
    if (closestSphereIndex < 0)
        return missHit(ray);

    return closestHit(ray, hitDistance, closestSphereIndex);
}

Renderer::HitPayload Renderer::closestHit(const Ray& ray, const float hitDistance, const uint32_t objectIndex) const {
    HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    Sphere* closestSphere = m_ActiveScene->GetSpheres()[objectIndex].get();
    const auto intersection = closestSphere->Hit(ray);
    payload.WorldPosition = intersection.FirstIntersection;
    payload.WorldNormal = closestSphere->NormalAtPoint(intersection.FirstIntersection);
    return payload;
}

Renderer::HitPayload Renderer::missHit(const Ray& ray)
{
    HitPayload payload = {};
    payload.HitDistance = -1.0f;
    return payload;
}