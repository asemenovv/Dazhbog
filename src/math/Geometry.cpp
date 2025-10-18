#include "Geometry.h"

#include "glm/gtx/norm.inl"

Sphere::Sphere(const float radius, const uint32_t materialIndex, const glm::vec3 &center)
    : m_Center(center), m_Radius(radius), m_MaterialIndex(materialIndex) {
}

ModelIntersections Sphere::Hit(const Ray &ray) const {
    // see https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/simplifyingtheray-sphereintersectioncode
    const glm::vec3 oc = m_Center - ray.Origin;
    const float a = glm::length2(ray.Direction);
    const float h = glm::dot(ray.Direction, oc);
    const float c = glm::length2(oc) - m_Radius * m_Radius;

    const float discriminant = h * h - a * c;
    if (discriminant < 0) {
        return ModelIntersections(0);
    }
    const float sqrtDiscr = sqrt(discriminant);
    const float t1 = (h - sqrtDiscr) / a;
    if (discriminant <= 1e-6f) {
        return ModelIntersections(1, ray.PointAt(t1), glm::dvec3(0.0), t1);
    }
    const float t2 = (h + sqrtDiscr) / a;
    return ModelIntersections(2, ray.PointAt(t1), ray.PointAt(t2), std::min(t1, t2));
}

glm::vec3 Sphere::NormalAtPoint(const glm::vec3 &point) const {
    return glm::normalize(point - m_Center);
}

void Sphere::MoveTo(const glm::vec3 &point) {
    m_Center = point;
}
