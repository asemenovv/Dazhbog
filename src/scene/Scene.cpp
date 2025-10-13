#include "Scene.h"

void Scene::Add(const Model &model, const Material &material) {
    SceneObject sceneObject = {
        .model = model,
        .material = material
    };
    m_SceneObjects.emplace_back(sceneObject);
}
