#include "Scene.h"

Scene::Scene() = default;

void Scene::Add(Sphere* sphere) {
    m_Spheres.emplace_back(std::unique_ptr<Sphere>(sphere));
}

Sphere* Scene::FindNearestSphere(const Ray& ray)
{
    Sphere* nearestSphere = nullptr;
    float nearestSphereDistance = FLT_MAX;
    for (std::unique_ptr<Sphere>& sphere : m_Spheres)
    {
        const auto sphereIntersections = sphere->Intersects(ray);
        if (sphereIntersections.NumberOfIntersections > 0
            && sphereIntersections.DistanceToNearest < nearestSphereDistance)
        {
            nearestSphereDistance = sphereIntersections.DistanceToNearest;
            nearestSphere = sphere.get();
        }
    }
    return nearestSphere;
}

std::vector<std::unique_ptr<Sphere>>& Scene::GetSpheres()
{
    return m_Spheres;
}
