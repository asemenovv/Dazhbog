#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const float verticalFOV, const float nearClip, const float farClip, const glm::vec2 viewportSize)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip),
    m_ForwardDirection({0, 0, -1}), m_Position({0, 0, 6}) {
    OnResize(viewportSize.x, viewportSize.y);
}

void Camera::OnResize(const uint32_t width, const uint32_t height) {
    if (width == m_ViewportWidth && height == m_ViewportHeight) {
        return;
    }
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    RecalculateProjection();
    RecalculateRayDirections();
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

void Camera::RecalculateRayDirections() {
    m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

    for (uint32_t y = 0; y < m_ViewportHeight; y++) {
        for (uint32_t x = 0; x < m_ViewportWidth; x++) {
            glm::vec2 coord = {
                static_cast<float>(x) / static_cast<float>(m_ViewportWidth),
                static_cast<float>(y) / static_cast<float>(m_ViewportHeight)
            };
            coord = coord * 2.0f - 1.0f; // -1 -> 1

            glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
            glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
            m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
        }
    }
}

void Camera::MoveForward(const float stepAmount) {
    m_Position += m_ForwardDirection * stepAmount;
    RecalculateRayDirections();
    RecalculateView();
}

void Camera::MoveRight(float stepAmount) {
    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    const glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);
    m_Position += rightDirection * stepAmount;
    RecalculateRayDirections();
    RecalculateView();
}
