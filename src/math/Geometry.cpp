#include "Geometry.h"

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtx/norm.inl"

Sphere::Sphere(const float radius, const uint32_t materialIndex, const glm::vec3 &center)
    : m_Center(center), m_Radius(radius), m_MaterialIndex(materialIndex) {
}

HitPayload Sphere::Hit(const Ray &ray, Interval tBoundaries) const {
    // see https://raytracing.github.io/books/RayTracingInOneWeekend.html#surfacenormalsandmultipleobjects/simplifyingtheray-sphereintersectioncode
    const glm::vec3 oc = m_Center - ray.Origin;
    const float a = glm::length2(ray.Direction);
    const float h = glm::dot(ray.Direction, oc);
    const float c = glm::length2(oc) - m_Radius * m_Radius;

    const float discriminant = h * h - a * c;
    if (discriminant < 0) {
        return {.DidCollide = false};
    }

    const float sqrtDiscr = sqrt(discriminant);

    auto root = (h - sqrtDiscr) / a;
    if (!tBoundaries.Surrounds(root)) {
        root = (h + sqrtDiscr) / a;
        if (!tBoundaries.Surrounds(root))
            return {.DidCollide = false};
    }

    HitPayload hitRecord{};
    hitRecord.DidCollide = true;
    hitRecord.HitDistance = root;
    hitRecord.WorldPosition = ray.PointAt(root);
    hitRecord.SetFaceNormal(ray, NormalAtPoint(hitRecord.WorldPosition));
    return hitRecord;
}

glm::vec3 Sphere::NormalAtPoint(const glm::vec3 &point) const {
    return (point - m_Center) / m_Radius;
}

void Sphere::MoveTo(const glm::vec3 &point) {
    m_Center = point;
}

Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const uint32_t materialIndex)
    : m_A(a), m_B(b), m_C(c), m_MaterialIndex(materialIndex) {
}

HitPayload Triangle::Hit(const Ray &ray, Interval tBoundaries) const {
    glm::vec3 E1 = m_B - m_A;
    glm::vec3 E2 = m_C - m_A;
    glm::vec3 normal = Normal();

    const float determinant = -glm::dot(ray.Direction, normal);
    if (determinant < 0.0f) {
        return {.DidCollide = false};
    }
    float invDeterminant = 1 / determinant;
    const glm::vec3 AO = ray.Origin - m_A;
    glm::vec3 DAO = glm::cross(AO, ray.Direction);

    float t = glm::dot(AO, normal) * invDeterminant;
    if (t < 0.0f || !tBoundaries.Surrounds(t)) {
        return {.DidCollide = false};
    }
    float u = glm::dot(E2, DAO) * invDeterminant;
    if (u < 0.0f) {
        return {.DidCollide = false};
    }
    float v = -glm::dot(E1, DAO) * invDeterminant;
    if (v < 0.0f) {
        return {.DidCollide = false};
    }
    float w = 1 - u - v;
    if (w < 0.0f) {
        return {.DidCollide = false};
    }

    HitPayload hitRecord{};
    hitRecord.DidCollide = true;
    hitRecord.HitDistance = t;
    hitRecord.WorldPosition = ray.PointAt(t);
    hitRecord.WorldNormal = glm::normalize(normal);
    return hitRecord;
}

glm::vec3 Triangle::Normal() const {
    return glm::cross(m_B - m_A, m_C - m_A);
}

uint32_t Triangle::GetMaterialIndex() const {
    return m_MaterialIndex;
}

Cube::Cube(const glm::mat4 &transform, const uint32_t materialIndex) : m_MaterialIndex(materialIndex) {
    constexpr float s = 0.5f;
    auto v000 = glm::vec3(transform * glm::vec4(-s, -s, -s, 1.0));
    auto v001 = glm::vec3(transform * glm::vec4(-s, -s, s, 1.0));
    auto v010 = glm::vec3(transform * glm::vec4(-s, s, -s, 1.0));
    auto v011 = glm::vec3(transform * glm::vec4(-s, s, s, 1.0));
    auto v100 = glm::vec3(transform * glm::vec4(s, -s, -s, 1.0));
    auto v101 = glm::vec3(transform * glm::vec4(s, -s, s, 1.0));
    auto v110 = glm::vec3(transform * glm::vec4(s, s, -s, 1.0));
    auto v111 = glm::vec3(transform * glm::vec4(s, s, s, 1.0));

    m_Triangles.emplace_back(Triangle(v100, v101, v001, materialIndex));
    m_Triangles.emplace_back(Triangle(v100, v001, v000, materialIndex));
    m_Triangles.emplace_back(Triangle(v011, v111, v110, materialIndex));
    m_Triangles.emplace_back(Triangle(v011, v110, v010, materialIndex));
    m_Triangles.emplace_back(Triangle(v001, v011, v010, materialIndex));
    m_Triangles.emplace_back(Triangle(v001, v010, v000, materialIndex));
    m_Triangles.emplace_back(Triangle(v110, v111, v101, materialIndex));
    m_Triangles.emplace_back(Triangle(v110, v101, v100, materialIndex));
    m_Triangles.emplace_back(Triangle(v010, v110, v100, materialIndex));
    m_Triangles.emplace_back(Triangle(v010, v100, v000, materialIndex));
    m_Triangles.emplace_back(Triangle(v101, v111, v011, materialIndex));
    m_Triangles.emplace_back(Triangle(v101, v011, v001, materialIndex));
}

uint32_t Cube::GetMaterialIndex() const {
    return m_MaterialIndex;
}

HitPayload Cube::Hit(const Ray &ray, const Interval tBoundaries) const {
    for (auto &triangle : m_Triangles) {
        if (const HitPayload payload = triangle.Hit(ray, tBoundaries); payload.DidCollide) {
            return payload;
        }
    }
    return { .DidCollide = false };
}
