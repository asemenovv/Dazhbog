#pragma once

#include <vector>
#include <memory>

#include "math/Geometry.h"
#include "render/Material.h"

class Scene {
public:
    Scene();

    ~Scene() = default;

    void Add(Hittable *sphere);

    void Add(Material *material);

    [[nodiscard]] std::vector<std::unique_ptr<Hittable> > &GetHittableObjects();

    [[nodiscard]] std::vector<std::unique_ptr<Material> > &GetMaterials();

private:
    std::vector<std::unique_ptr<Hittable> > m_HittableObjects;
    std::vector<std::unique_ptr<Material> > m_Materials;
};
