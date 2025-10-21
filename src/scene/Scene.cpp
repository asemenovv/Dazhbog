#include "Scene.h"

#include "render/Material.h"

Scene::Scene() {
    Add(new DiffuseMaterial({0.2, 0.2, 0.2}));
}

void Scene::Add(Hittable* sphere)
{
    m_HittableObjects.emplace_back(std::unique_ptr<Hittable>(sphere));
}

void Scene::Add(Material *material)
{
    m_Materials.emplace_back(std::unique_ptr<Material>(material));
}

std::vector<std::unique_ptr<Hittable>>& Scene::GetHittableObjects()
{
    return m_HittableObjects;
}

std::vector<std::unique_ptr<Material>> & Scene::GetMaterials()
{
    return m_Materials;
}
