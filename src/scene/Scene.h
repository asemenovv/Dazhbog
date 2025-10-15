#pragma once

#include <vector>
#include <memory>

#include "math/Geometry.h"

class Scene {
public:
    Scene();

    ~Scene() = default;

    void Add(Sphere *sphere);

    void Add(Material *material);

    [[nodiscard]] std::vector<std::unique_ptr<Sphere> > &GetSpheres();

    [[nodiscard]] std::vector<std::unique_ptr<Material> > &GetMaterials();

private:
    std::vector<std::unique_ptr<Sphere> > m_Spheres;
    std::vector<std::unique_ptr<Material> > m_Materials;
};
