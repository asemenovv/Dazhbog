#include "Renderer.h"

#include "math/Geometry.h"
#include "scene/Scene.h"

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
    for (int x = 0; x < m_Width; x++)
    {
        for (int y = 0; y < m_Height; y++)
        {
            const auto color = glm::clamp(perPixel(x, y), glm::vec4(0.0), glm::vec4(1.0f));
            m_ImageData[y * m_Width + x] = Utils::Vec4ToRGBA8(color);
        }
    }
}

void Renderer::OnResize(const uint32_t width, const uint32_t height) {
    m_Width = width;
    m_Height = height;
    delete[] m_ImageData;
    m_ImageData = new std::uint32_t[width * height];
}

glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
float AMBIENT = 0.1f;

glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + m_Width * y];
    const HitPayload payload = traceRay(ray);

    if (payload.HitDistance < 0.0f)
        return {0.0f, 0.0f, 0.0f, 1.0f};

    Sphere* sphere = m_ActiveScene->GetSpheres()[payload.ObjectIndex].get();
    const float diffuseIntensity = std::max(glm::dot(payload.WorldNormal, -LIGHT_DIRECTION), 0.0f);
    const auto ambient = AMBIENT * sphere->GetMaterial().Albedo;
    const auto diffuse = diffuseIntensity * sphere->GetMaterial().Albedo;
    return {ambient + diffuse, 1.0f};
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
        if (intersects.DistanceToNearest < hitDistance) {
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