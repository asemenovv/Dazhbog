#include "Renderer.h"

#include "math/Geometry.h"
#include "scene/Scene.h"
#include "wallnut/Random.h"

namespace Utils {
    static uint32_t Vec4ToRGBA8(const glm::vec4 color) {
        const auto r = static_cast<uint8_t>(color.r * 255.0f);
        const auto g = static_cast<uint8_t>(color.g * 255.0f);
        const auto b = static_cast<uint8_t>(color.b * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);
        return a << 24 | b << 16 | g << 8 | r;
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
    for (int x = 0; x < m_Width; x++)
    {
        for (int y = 0; y < m_Height; y++)
        {
            glm::vec4 color = perPixel(x, y);
            m_AccumulationData[x + y * m_Width] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_Width];
            accumulatedColor /= static_cast<float>(m_FrameIndex);

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0), glm::vec4(1.0f));
            m_ImageData[y * m_Width + x] = Utils::Vec4ToRGBA8(accumulatedColor);
        }
    }

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
}

glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
glm::vec3 SKY_COLOR(0.6f, 0.7f, 0.9f);
float AMBIENT = 0.1f;

glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + m_Width * y];

    glm::vec3 color(0.0f);
    float MULTIPLIER = 1.0f;

    constexpr int BOUNCES = 5;
    for (int i = 0; i < BOUNCES; i++) {
        const HitPayload payload = traceRay(ray);
        if (payload.HitDistance < 0.0f) {
            color += SKY_COLOR * MULTIPLIER;
            break;
        }

        Sphere* sphere = m_ActiveScene->GetSpheres()[payload.ObjectIndex].get();
        Material* material = m_ActiveScene->GetMaterials()[sphere->GetMaterialIndex()].get();
        const float diffuseIntensity = std::max(glm::dot(payload.WorldNormal, -LIGHT_DIRECTION), 0.0f);
        const auto ambient = AMBIENT * material->Albedo;
        const auto diffuse = diffuseIntensity * material->Albedo;
        const auto sphereColor = diffuse;
        color += MULTIPLIER * sphereColor;
        MULTIPLIER *= 0.5f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(
            ray.Direction,
            payload.WorldNormal + material->Roughness * Walnut::Random::Vec3(-0.5f, 0.5f)
        );
    }
    return {color, 1.0f};
}

Renderer::HitPayload Renderer::traceRay(const Ray& ray)
{
    int closestSphereIndex = -1;
    float hitDistance = std::numeric_limits<float>::max();
    for (int i = 0; i < m_ActiveScene->GetSpheres().size(); i++) {
        const Sphere* sphere = m_ActiveScene->GetSpheres()[i].get();
        const ModelIntersections intersects = sphere->Intersects(ray);
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
    const auto intersection = closestSphere->Intersects(ray);
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