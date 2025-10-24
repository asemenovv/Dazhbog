#include "Application.h"

#include <QKeyEvent>

#include "Input.h"
#include "glm/ext/matrix_transform.hpp"

float ROTATION_SPEED = 0.05f;
float CAMERA_MOVE_SPEED = 0.02f;
float CAMERA_ROTATION_SPEED = 0.002f;

Application::Application(int argc, char *argv[]) : m_RenderTimeMs(0) {
    m_QtApplication = std::make_unique<QApplication>(argc, argv);
    QApplication::setApplicationName("Dazhbog");
    QApplication::setOrganizationName("Mythological Worlds");

    m_Window = std::make_unique<MainWindow>([&](const int width, const int height) {
        this->OnCanvasResize(width, height);
    });
    m_Window->resize(1024, 768);
    m_Window->show();

    m_RenderTimer = std::make_unique<QTimer>(m_Window.get());
    connect(m_RenderTimer.get(), &QTimer::timeout, this, &Application::OnRender);
    m_RenderTimer->start();

    m_Camera = std::make_unique<Camera>(45.0, 0.1, 100.0, m_Window->GetCanvasSize());
    m_Camera->PlaceInWorld({0.4, 4.3, 11}, {0.2, 0.4, 0.9});
    m_Window->UpdateCameraLocation(m_Camera->GetPosition(), m_Camera->GetDirection());
    m_Window->SetButtonHandler([this](const MainWindow::ButtonAction action)
    {
        if (action == MainWindow::ButtonAction::Dump)
        {
            m_Renderer->DumpFramesToDisc("/Users/alexeysemenov/CLionProjects/Dazhbog/dump");
        }
    });
    SetupScene();
    m_Renderer = std::make_unique<Renderer>(m_Camera.get(), m_Scene.get(), m_Window->GetCanvasSize());
    m_Renderer->GetSettings().Exposure = 4.0f;
    m_Renderer->GetSettings().FramesToAccumulate = 500;
    m_Renderer->GetSettings().RayBounces = 10;
    m_Renderer->GetSettings().BloomThreshold = 0.5f;
    m_Renderer->GetSettings().BloomLevels = 4;
}

int Application::Run() {
    m_QtApplication->installEventFilter(this);
    return QApplication::exec();
}

void Application::SetupScene() {
    m_Scene = std::make_unique<Scene>();
    const auto greenMat = m_Scene->Add(new LambertMaterial({0.8, 0.8, 0.0}));
    const auto blueMat = m_Scene->Add(new LambertMaterial({0.1, 0.2, 0.5}));
    const auto silverMat = m_Scene->Add(new MetalMaterial({0.8, 0.8, 0.8}, 0.04));
    const auto goldenMat = m_Scene->Add(new MetalMaterial({0.8, 0.6, 0.2}, 0.0));
    const auto lightMat = m_Scene->Add(new DiffuseLightMaterial({1.0, 0.706, 0.422}, 1.0));

    //Floor
    m_Scene->Add(new Triangle(
        {-1000.0f, 0.0f, 1000.0f},
        {1000.0f, 0.0f, -1000.0f},
        {-1000.0f, 0.0f, -1000.0f},
        greenMat));
    m_Scene->Add(new Triangle(
        {-1000.0f, 0.0f, 1000.0f},
        {1000.0f, 0.0f, 1000.0f},
        {1000.0f, 0.0f, -1000.0f},
        greenMat));

    m_Scene->Add(new Sphere(2.0f, blueMat, glm::vec3(0.0f, 2.0f, 0.0f)));
    m_Scene->Add(new Sphere(10.0f, lightMat, glm::vec3(0.0f, 40.0f, -10.0f)));

    const glm::mat4 scale = glm::scale(glm::mat4(1.0), {10.0f, 10.0f, 10.0f});
    constexpr glm::mat4 translateSilver = glm::translate(glm::mat4(1.0), {20.0f, 5.0f, 20.0f});
    constexpr glm::mat4 translateGold = glm::translate(glm::mat4(1.0), {-20.0f, 5.0f, 20.0f});
    m_Scene->Add(new Cube(translateSilver * scale, silverMat));
    m_Scene->Add(new Cube(translateGold * scale, goldenMat));
}

void Application::OnRender() {
    QElapsedTimer timer;
    timer.start();
    const Renderer::RenderingStatus renderingStatus = m_Renderer->Render();
    m_RenderTimeMs = renderingStatus.FrameRenderTime;
    m_Window->UpdateRenderTime(renderingStatus.FrameRenderTime, renderingStatus.SceneRenderTime);
    m_Window->UpdateFrame(renderingStatus.FrameIndex);

    const auto imageData = m_Renderer->GetFinalImageData();
    m_Window->ShowImage(imageData);
    OnUpdate(static_cast<float>(renderingStatus.FrameRenderTime));
}

void Application::OnUpdate(const float deltaTime) const {
    bool cameraMoved = false;
    if (Input::IsKeyPressed(Input::Key::W)) {
        m_Camera->MoveForward(static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::S)) {
        m_Camera->MoveForward(-static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::D)) {
        m_Camera->MoveRight(static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::A)) {
        m_Camera->MoveRight(-static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::E)) {
        m_Camera->MoveUp(static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::Q)) {
        m_Camera->MoveUp(-static_cast<float>(deltaTime) * CAMERA_MOVE_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::ArrowRight)) {
        m_Camera->Yaw(static_cast<float>(deltaTime) * CAMERA_ROTATION_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::ArrowLeft)) {
        m_Camera->Yaw(-static_cast<float>(deltaTime) * CAMERA_ROTATION_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::ArrowUp)) {
        m_Camera->Pitch(static_cast<float>(deltaTime) * CAMERA_ROTATION_SPEED);
        cameraMoved = true;
    }
    if (Input::IsKeyPressed(Input::Key::ArrowDown)) {
        m_Camera->Pitch(-static_cast<float>(deltaTime) * CAMERA_ROTATION_SPEED);
        cameraMoved = true;
    }

    if (cameraMoved) {
        m_Camera->Refresh();
        m_Window->UpdateCameraLocation(m_Camera->GetPosition(), m_Camera->GetDirection());
        m_Renderer->ResetFrameIndex();
    }
}

void Application::OnCanvasResize(const int width, const int height) const {
    if (m_Renderer) {
        m_Renderer->OnResize(width, height);
        m_Camera->OnResize(width, height);
    }
}
