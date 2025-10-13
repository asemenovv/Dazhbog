#include "Geometry.h"

Sphere::Sphere(const float radius, const Material material, const glm::vec3& center)
    : m_Center(center), m_Radius(radius), m_Material(material) {
}

ModelIntersections Sphere::Intersects(const Ray& ray) const {
    const float a = glm::dot(ray.Direction, ray.Direction);
    const float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, m_Center));
    const float c = glm::dot(ray.Origin, ray.Origin)
        + glm::dot(m_Center, m_Center)
        - 2.0f * glm::dot(ray.Origin, m_Center)
        - m_Radius * m_Radius;

    // D = b^2 - 4ac
    const float discriminant = (b * b) - 4 * a * c;
    if (discriminant < 0) {
        return ModelIntersections(0);
    }
    const float t1 = (-b - sqrt(discriminant)) / (2 * a);
    if (discriminant == 0) {
        return ModelIntersections(1, ray.PointAt(t1), glm::dvec3(0.0), t1);
    }
    const float t2 = (-b + sqrt(discriminant)) / (2 * a);
    return ModelIntersections(2, ray.PointAt(t1), ray.PointAt(t2), std::min(t1, t2));
}

glm::vec3 Sphere::NormalAtPoint(const glm::vec3& point) const {
    return glm::normalize(point - m_Center);
}
