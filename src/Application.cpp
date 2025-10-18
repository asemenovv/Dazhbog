#include "Application.h"

#include <QKeyEvent>

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"

Application::Application(int argc, char *argv[]): m_RenderTimeMs(0) {
    m_QtApplication = std::make_unique<QApplication>(argc, argv);
    QApplication::setApplicationName("Dazhbog");
    QApplication::setOrganizationName("Mythological Worlds");

    m_Window = std::make_unique<MainWindow>([&](const int width, const int height) {
        this->OnCanvasResize(width, height);
    });
    m_Window->resize(1024,768);
    m_Window->show();

    m_RenderTimer = std::make_unique<QTimer>(m_Window.get());
    connect(m_RenderTimer.get(), &QTimer::timeout, this, &Application::OnRender);
    m_RenderTimer->start();

    m_Camera = std::make_unique<Camera>(45.0, 0.1, 100.0, m_Window->GetCanvasSize());
    m_Camera->PlaceInWorld({0.4, 4.3, 11}, {0.2, 0.4, 0.9});
    m_Window->UpdateCameraLocation(m_Camera->GetPosition(), m_Camera->GetDirection());
    SetupScene();
    m_Renderer = std::make_unique<Renderer>(m_Camera.get(), m_Scene.get(), m_Window->GetCanvasSize());
}

int Application::Run() {
    m_QtApplication->installEventFilter(this);
    return QApplication::exec();
}

void Application::SetupScene() {
    m_Scene = std::make_unique<Scene>();
    m_Scene->Add(new Material({ // Blue Floor 1
        .Albedo = {0.2, 0.3, 1.0},
        .Roughness = 0.1f
    }));
    m_Scene->Add(new Material({ // Pinky Sphere 2
        .Albedo = {1.0, 0.0, 1.0},
        .Roughness = 1.0f,
        .EmissionColor = {0.0, 1.0, 0.0},
        .EmissionPower = 0.0f
    }));
    m_Scene->Add(new Material({ // Emissive Sphere 3
        .Albedo = {0.8, 0.5, 0.2},
        .Roughness = 0.1f,
        .EmissionColor = {0.8, 0.5, 0.2},
        .EmissionPower = 10.0f
    }));
    m_Scene->Add(new Triangle({-1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, -1000.0f}, {-1000.0f, 0.0f, -1000.0f}, 1));
    m_Scene->Add(new Triangle({-1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, -1000.0f}, 1));
    m_Scene->Add(new Sphere(2.0f, 2, glm::vec3(0.0f, 2.0f, 0.0f)));
    m_Scene->Add(new Sphere(10.0f, 3, glm::vec3(30.0f, 20.0f, -20.0f)));
    constexpr int spheresCount = 8;
    for (int i = 0; i < spheresCount; i++)
    {
        const float x = 5.0f * std::cos(2.0f * glm::pi<float>() * static_cast<float>(i) / spheresCount);
        const float z = 5.0f * std::sin(2.0f * glm::pi<float>() * static_cast<float>(i) / spheresCount);
        auto center = glm::vec3(x, 0.0f, z);
        // m_Scene->Add(new Sphere(0.125f, spheresMaterial, center));
    }
}

void Application::OnRender() {
    QElapsedTimer timer;
    timer.start();
    m_Renderer->Render();
    m_RenderTimeMs = timer.elapsed();
    m_Window->UpdateRenderTime(m_RenderTimeMs);

    const auto imageData = m_Renderer->GetFinalImageData();
    m_Window->ShowImage(imageData);
    OnUpdate(static_cast<float>(m_RenderTimeMs));
}

float ROTATION_SPEED = 0.05f;

void Application::OnUpdate(const float deltaTime) {
    /*for (const std::unique_ptr<Sphere>& sphere : m_Scene->GetSpheres())
    {
        auto rotation = glm::rotate(
            glm::mat4{1.0f},
            glm::radians(ROTATION_SPEED * deltaTime),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        sphere->MoveTo(glm::vec3(rotation * sphere->GetCenter()));
    }*/
}

void Application::OnCanvasResize(const int width, const int height) const
{
    if (m_Renderer) {
        m_Renderer->OnResize(width, height);
        m_Camera->OnResize(width, height);
    }
}

float CAMERA_SPEED = 0.02f;

bool Application::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        bool cameraMoved = false;
        const auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_S) {
            m_Camera->MoveForward(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_W) {
            m_Camera->MoveForward(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_D) {
            m_Camera->MoveRight(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_A) {
            m_Camera->MoveRight(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_E) {
            m_Camera->MoveUp(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_Q) {
            m_Camera->MoveUp(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_Right) {
            m_Camera->Yaw(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_Left) {
            m_Camera->Yaw(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_Up) {
            m_Camera->Pitch(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (keyEvent->key() == Qt::Key_Down) {
            m_Camera->Pitch(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
            cameraMoved = true;
        }
        if (cameraMoved) {
            m_Camera->Refresh();
            m_Window->UpdateCameraLocation(m_Camera->GetPosition(), m_Camera->GetDirection());
            m_Renderer->ResetFrameIndex();
        }
        return false;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        qDebug() << "Global click at:" << mouseEvent->position();
    }
    return QObject::eventFilter(obj, event);
}
