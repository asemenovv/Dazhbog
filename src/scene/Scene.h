#pragma once

#include <vector>
#include <memory>

#include "math/Geometry.h"

class Scene {
public:
    Scene();
    ~Scene() = default;

    void Add(Sphere* sphere);

    Sphere* FindNearestSphere(const Ray& ray);

    [[nodiscard]] std::vector<std::unique_ptr<Sphere>>& GetSpheres();
private:
    std::vector<std::unique_ptr<Sphere>> m_Spheres;
};
