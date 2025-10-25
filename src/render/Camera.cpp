#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const float verticalFOV, const float nearClip, const float farClip, const glm::vec2 viewportSize)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip),
    m_ForwardDirection({0, 0, -1 }), m_Position({0, 0, 0}) {
    OnResize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
}

void Camera::PlaceInWorld(const glm::vec3& position, const glm::vec3& direction)
{
    m_Position = position;
    m_ForwardDirection = direction;
    RecalculateView();
}

void Camera::OnResize(const uint32_t width, const uint32_t height) {
    if (width == m_ViewportWidth && height == m_ViewportHeight) {
        return;
    }
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    RecalculateProjection();
    RecalculateView();
}

void Camera::RecalculateProjection() {
    m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), static_cast<float>(m_ViewportWidth),
        static_cast<float>(m_ViewportHeight), m_NearClip, m_FarClip);
    m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView() {
    m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
    m_InverseView = glm::inverse(m_View);
}

Ray Camera::GetRay(const float pixelX, const float pixelY) const {
    const float u = pixelX / static_cast<float>(m_ViewportWidth);
    const float v = pixelY / static_cast<float>(m_ViewportHeight);

    const glm::vec2 ndc = glm::vec2(u, v) * 2.0f - 1.0f;
    const glm::vec4 target = m_InverseProjection * glm::vec4(ndc.x, ndc.y, 1.0f, 1.0f);
    const glm::vec3 dirView = glm::normalize(glm::vec3(target) / target.w);
    auto dirWorld = glm::vec3(m_InverseView * glm::vec4(dirView, 0.0f));
    dirWorld = glm::normalize(dirWorld);

    const Ray ray(m_Position, dirWorld);
    return ray;
}

void Camera::MoveForward(const float stepAmount)
{
    m_Position += glm::normalize(m_ForwardDirection) * stepAmount;
    RecalculateView();
}

void Camera::MoveRight(const float stepAmount)
{
    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    const glm::vec3 rightDirection = glm::normalize(glm::cross(m_ForwardDirection, upDirection));
    m_Position += rightDirection * stepAmount;
    RecalculateView();
}

void Camera::MoveUp(const float stepAmount)
{
    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    m_Position += upDirection * stepAmount;
    RecalculateView();
}

void Camera::Pitch(const float angle)
{
    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    const glm::vec3 rightDirection = glm::normalize(glm::cross(m_ForwardDirection, upDirection));
    const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, rightDirection);
    m_ForwardDirection = glm::normalize(glm::vec3(rotation * glm::vec4(m_ForwardDirection, 1.0f)));
    RecalculateView();
}

void Camera::Yaw(const float angle)
{
    constexpr glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, worldUp);
    m_ForwardDirection = glm::normalize(glm::vec3(rotation * glm::vec4(m_ForwardDirection, 1.0f)));
    RecalculateView();
}
