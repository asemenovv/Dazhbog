#include "Scene.h"

Scene::Scene() {
    Add(new Material({
        .Albedo = {0.2, 0.2, 0.2},
        .Roughness = 1.0f
    }));
}

void Scene::Add(Sphere* sphere)
{
    m_Spheres.emplace_back(std::unique_ptr<Sphere>(sphere));
}

void Scene::Add(Material *material)
{
    m_Materials.emplace_back(std::unique_ptr<Material>(material));
}

std::vector<std::unique_ptr<Sphere>>& Scene::GetSpheres()
{
    return m_Spheres;
}

std::vector<std::unique_ptr<Material>> & Scene::GetMaterials()
{
    return m_Materials;
}
