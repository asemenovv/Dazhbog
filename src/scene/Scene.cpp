#include "Scene.h"

#include "render/Material.h"

Scene::Scene() {
    Add(new LambertMaterial({0.2, 0.2, 0.2}));
}

uint32_t Scene::Add(Hittable* sphere)
{
    m_HittableObjects.emplace_back(std::unique_ptr<Hittable>(sphere));
    return m_HittableObjects.size() - 1;
}

uint32_t Scene::Add(Material *material)
{
    m_Materials.emplace_back(std::unique_ptr<Material>(material));
    return m_Materials.size() - 1;
}

std::vector<std::unique_ptr<Hittable>>& Scene::GetHittableObjects()
{
    return m_HittableObjects;
}

std::vector<std::unique_ptr<Material>> & Scene::GetMaterials()
{
    return m_Materials;
}
