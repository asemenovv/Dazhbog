#include "Hittable.h"

void HitPayload::SetFaceNormal(const Ray &ray, const glm::vec3 &outwardNormal) {
    FrontFace = glm::dot(ray.Direction, outwardNormal) < 0;
    WorldNormal = FrontFace ? outwardNormal : -outwardNormal;
}
