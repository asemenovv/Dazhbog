#include "Scene.h"

Scene::Scene() = default;

void Scene::Add(Sphere* sphere) {
    m_Spheres.emplace_back(std::unique_ptr<Sphere>(sphere));
}

std::vector<std::unique_ptr<Sphere>>& Scene::GetSpheres()
{
    return m_Spheres;
}
