#include "Geometry.h"

#include "glm/gtx/norm.inl"

Sphere::Sphere(const float radius, const uint32_t materialIndex, const glm::vec3 &center)
    : m_Center(center), m_Radius(radius), m_MaterialIndex(materialIndex) {
}

HitPayload Sphere::Hit(const Ray &ray, float tMin, float tMax) const {
    // see https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/simplifyingtheray-sphereintersectioncode
    const glm::vec3 oc = m_Center - ray.Origin;
    const float a = glm::length2(ray.Direction);
    const float h = glm::dot(ray.Direction, oc);
    const float c = glm::length2(oc) - m_Radius * m_Radius;

    const float discriminant = h * h - a * c;
    if (discriminant < 0) {
        return {.IsHit = false};
    }

    const float sqrtDiscr = sqrt(discriminant);

    auto root = (h - sqrtDiscr) / a;
    if (root <= tMin || root >= tMax) {
        root = (h + sqrtDiscr) / a;
        if (root <= tMin || root >= tMax)
            return {.IsHit = false};
    }

    HitPayload hitRecord{};
    hitRecord.IsHit = true;
    hitRecord.HitDistance = root;
    hitRecord.WorldPosition = ray.PointAt(root);
    hitRecord.WorldNormal = NormalAtPoint(hitRecord.WorldPosition);
    return hitRecord;
}

glm::vec3 Sphere::NormalAtPoint(const glm::vec3 &point) const {
    return (point - m_Center) / m_Radius;
}

void Sphere::MoveTo(const glm::vec3 &point) {
    m_Center = point;
}
