#pragma once
#include <memory>
#include <QApplication>
#include <QTimer>

#include "MainWindow.h"
#include "render/Camera.h"
#include "render/Renderer.h"
#include "scene/Scene.h"

class Application : public QObject {
    Q_OBJECT
public:
    Application(int argc, char *argv[]);

    ~Application() override = default;

    int Run();

    void SetupScene();

    void OnRender();

    void OnUpdate(float deltaTime);

    void OnCanvasResize(int width, int height) const;

protected slots:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    std::unique_ptr<QApplication> m_QtApplication;

    std::unique_ptr<MainWindow> m_Window;

    std::unique_ptr<Renderer> m_Renderer;

    std::unique_ptr<Camera> m_Camera;

    std::unique_ptr<Scene> m_Scene;

    std::unique_ptr<QTimer> m_RenderTimer;

    int64_t m_RenderTimeMs;

    void addBox(int materialIndex);
};
