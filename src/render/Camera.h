#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera(float verticalFOV, float nearClip, float farClip, glm::vec2 viewportSize);

    void PlaceInWorld(const glm::vec3& position, const glm::vec3& direction);

    void OnResize(uint32_t width, uint32_t height);

    void RecalculateProjection();

    void RecalculateView();

    void RecalculateRayDirections();

    const glm::vec3& GetPosition() const { return m_Position; }

    const glm::vec3& GetDirection() const { return m_ForwardDirection; }

    const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

    void MoveForward(float stepAmount);

    void MoveRight(float stepAmount);

    void MoveUp(float stepAmount);

    void Pitch(float angle);

    void Yaw(float angle);

    void Refresh();

private:
    glm::mat4 m_Projection{ 1.0f };
    glm::mat4 m_View{ 1.0f };
    glm::mat4 m_InverseProjection{ 1.0f };
    glm::mat4 m_InverseView{ 1.0f };

    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    glm::vec3 m_ForwardDirection;
    glm::vec3 m_Position;

    std::vector<glm::vec3> m_RayDirections;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
