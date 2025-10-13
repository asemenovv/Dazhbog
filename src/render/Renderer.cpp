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

Renderer::Renderer(const glm::vec2 viewportSize): m_ImageData(nullptr), m_Width(0), m_Height(0) {
    OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
}

void Renderer::Render(Scene* scene, const Camera* camera) {
    Ray viewRay;
    viewRay.Origin = camera->GetPosition();
    for (int x = 0; x < m_Width; x++) {
        for (int y = 0; y < m_Height; y++) {
            viewRay.Direction = camera->GetRayDirections()[x + m_Width * y];
            const auto color = glm::clamp(this->traceRay(scene, viewRay), glm::vec4(0.0), glm::vec4(1.0f));
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

glm::vec4 Renderer::traceRay(Scene* scene, const Ray& viewRay) {
    if (Sphere* nearestSphere = scene->FindNearestSphere(viewRay); nearestSphere != nullptr)
    {
        const auto intersection = nearestSphere->Intersects(viewRay);
        const auto normal = nearestSphere->NormalAtPoint(intersection.FirstIntersection);
        const float diffuseIntensity = std::max(glm::dot(normal, -LIGHT_DIRECTION), 0.0f);
        const auto ambient = AMBIENT * nearestSphere->GetMaterial().Albedo;
        const auto diffuse = diffuseIntensity * nearestSphere->GetMaterial().Albedo;
        return {ambient + diffuse, 1.0f};
    }
    return {0.0, 0.0, 0.0, 1.0};
}
