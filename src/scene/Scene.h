#pragma once
#include <vector>

#include "render/Material.h"

struct Model {

};

struct SceneObject {
    Model model;
    Material material;
};

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void Add(const Model& model, const Material& material);

    std::vector<SceneObject> GetSceneObjects() const { return m_SceneObjects; }
private:
    std::vector<SceneObject> m_SceneObjects;
};
