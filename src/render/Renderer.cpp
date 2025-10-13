#include "Renderer.h"

#include "math/Geometry.h"

namespace Utils {
    static uint32_t Vec4ToRGBA8(const glm::vec4 color) {
        const auto r = static_cast<uint8_t>(color.r * 255.0f);
        const auto g = static_cast<uint8_t>(color.g * 255.0f);
        const auto b = static_cast<uint8_t>(color.b * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);
        return a << 24 | b << 16 | g << 8 | r;
    }
}

Renderer::Renderer(const glm::vec2 viewportSize): m_Width(0), m_Height(0) {
    OnResize(viewportSize.x, viewportSize.y);
}

void Renderer::Render(const Camera* camera) {
    Ray viewRay;
    viewRay.Origin = camera->GetPosition();
    for (int x = 0; x < m_Width; x++) {
        for (int y = 0; y < m_Height; y++) {
            viewRay.Direction = camera->GetRayDirections()[x + m_Width * y];
            const auto color = glm::clamp(this->traceRay(viewRay), glm::vec4(0.0), glm::vec4(1.0f));
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

glm::vec3 SPHERE_COLOR = glm::vec3{1.0, 0.0, 1.0};
glm::vec3 LIGHT_DIRECTION = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
float AMBIENT = 0.1f;

glm::vec4 Renderer::traceRay(const Ray& viewRay) {
    const Sphere sphere(0.5f, glm::vec3(0.0f, 0.0f, -1.0f));

    auto sphereIntersections = sphere.Intersects(viewRay);
    if (sphereIntersections.NumberOfIntersections > 0) {
        const auto normal = sphere.NormalAtPoint(sphereIntersections.FirstIntersection);
        float diffuseIntensity = std::max(glm::dot(normal, -LIGHT_DIRECTION), 0.0f);
        auto ambient = AMBIENT * SPHERE_COLOR;
        auto diffuse = diffuseIntensity * SPHERE_COLOR;
        return glm::vec4(ambient + diffuse, 1.0f);
    }
    return {0.0, 0.0, 0.0, 1.0};
}
