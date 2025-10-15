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
    m_Window->resize(800,600);
    m_Window->show();

    m_RenderTimer = std::make_unique<QTimer>(m_Window.get());
    connect(m_RenderTimer.get(), &QTimer::timeout, this, &Application::OnRender);
    m_RenderTimer->start();

    m_Camera = std::make_unique<Camera>(45.0, 0.1, 100.0, m_Window->GetCanvasSize());
    SetupScene();
    m_Renderer = std::make_unique<Renderer>(m_Camera.get(), m_Scene.get(), m_Window->GetCanvasSize());
}

int Application::Run() {
    m_QtApplication->installEventFilter(this);
    return QApplication::exec();
}

void Application::SetupScene() {
    m_Scene = std::make_unique<Scene>();
    constexpr Material pinkyMaterial = {
        .Albedo = {1.0, 0.0, 1.0}
    };
    constexpr Material blueMaterial = {
        .Albedo = {0.0, 0.0, 1.0}
    };
    m_Scene->Add(new Sphere(0.5f, pinkyMaterial, glm::vec3(0.0f, 0.0f, 0.0f)));
    constexpr int spheresCount = 4;
    for (int i = 0; i < spheresCount; i++)
    {
        const float x = 2.0f * std::cos(2.0f * glm::pi<float>() * static_cast<float>(i) / spheresCount);
        const float z = 2.0f * std::sin(2.0f * glm::pi<float>() * static_cast<float>(i) / spheresCount);
        auto center = glm::vec3(x, 0.0f, z);
        m_Scene->Add(new Sphere(0.125f, blueMaterial, center));
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
    for (const std::unique_ptr<Sphere>& sphere : m_Scene->GetSpheres())
    {
        auto rotation = glm::rotate(
            glm::mat4{1.0f},
            glm::radians(ROTATION_SPEED * deltaTime),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        sphere->MoveTo(glm::vec3(rotation * sphere->GetCenter()));
    }
}

void Application::OnCanvasResize(const int width, const int height) const
{
    if (m_Renderer) {
        m_Renderer->OnResize(width, height);
        m_Camera->OnResize(width, height);
    }
}

float CAMERA_SPEED = 0.2f;

bool Application::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        const auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_W) {
            m_Camera->MoveForward(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
        }
        else if (keyEvent->key() == Qt::Key_S) {
            m_Camera->MoveForward(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
        }
        else if (keyEvent->key() == Qt::Key_D) {
            m_Camera->MoveRight(static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
        }
        else if (keyEvent->key() == Qt::Key_A) {
            m_Camera->MoveRight(-static_cast<float>(m_RenderTimeMs) * CAMERA_SPEED);
        }
        return false;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        qDebug() << "Global click at:" << mouseEvent->position();
    }
    return QObject::eventFilter(obj, event);
}
