#include "Application.h"

#include <QKeyEvent>

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"

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
    SetupScene();
    m_Renderer = std::make_unique<Renderer>(m_Camera.get(), m_Scene.get(), m_Window->GetCanvasSize());
}

int Application::Run() {
    m_QtApplication->installEventFilter(this);
    return QApplication::exec();
}

void Application::SetupScene() {
    m_Scene = std::make_unique<Scene>();
    // Blue Floor 1
    m_Scene->Add(new DiffuseMaterial({0.2, 0.3, 1.0}, {0.0, 0.0, 0.0}, 0.0f));
    // Pinky Sphere 2
    m_Scene->Add(new DiffuseMaterial({1.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 0.0f));
    // Emissive Sphere 3
    m_Scene->Add(new DiffuseMaterial({0.8, 0.5, 0.2}, {0.8, 0.5, 0.2}, 20.0f));
    // Green Cube 4
    m_Scene->Add(new DiffuseMaterial({0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, 0.0f));

    //Floor
    m_Scene->Add(new Triangle({-1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, -1000.0f}, {-1000.0f, 0.0f, -1000.0f}, 1));
    m_Scene->Add(new Triangle({-1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, 1000.0f}, {1000.0f, 0.0f, -1000.0f}, 1));

    addBox(4);

    m_Scene->Add(new Sphere(2.0f, 2, glm::vec3(0.0f, 2.0f, 0.0f)));
    m_Scene->Add(new Sphere(10.0f, 3, glm::vec3(30.0f, 20.0f, -20.0f)));
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
}

void Application::OnCanvasResize(const int width, const int height) const {
    if (m_Renderer) {
        m_Renderer->OnResize(width, height);
        m_Camera->OnResize(width, height);
    }
}

float CAMERA_SPEED = 0.02f;

bool Application::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        bool cameraMoved = false;
        const auto *keyEvent = static_cast<QKeyEvent *>(event);
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
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug() << "Global click at:" << mouseEvent->position();
    }
    return QObject::eventFilter(obj, event);
}

void Application::addBox(int materialIndex) {
    const float s = 0.5f; // половина размера куба

    glm::mat4 scale = glm::scale(glm::mat4(1.0), {10.0f, 10.0f, 10.0f});
    glm::mat4 translate = glm::translate(glm::mat4(1.0), {20.0f, 5.0f, 20.0f});
    glm::mat4 transform = translate * scale;

    // Вершины куба
    glm::vec3 v000 = glm::vec3(transform * glm::vec4(-s, -s, -s, 1.0));
    glm::vec3 v001 = glm::vec3(transform * glm::vec4(-s, -s, s, 1.0));
    glm::vec3 v010 = glm::vec3(transform * glm::vec4(-s, s, -s, 1.0));
    glm::vec3 v011 = glm::vec3(transform * glm::vec4(-s, s, s, 1.0));
    glm::vec3 v100 = glm::vec3(transform * glm::vec4(s, -s, -s, 1.0));
    glm::vec3 v101 = glm::vec3(transform * glm::vec4(s, -s, s, 1.0));
    glm::vec3 v110 = glm::vec3(transform * glm::vec4(s, s, -s, 1.0));
    glm::vec3 v111 = glm::vec3(transform * glm::vec4(s, s, s, 1.0));

    // Низ (y = -s)
    m_Scene->Add(new Triangle(v100, v101, v001, materialIndex));
    m_Scene->Add(new Triangle(v100, v001, v000, materialIndex));

    // Верх (y = +s)
    m_Scene->Add(new Triangle(v011, v111, v110, materialIndex));
    m_Scene->Add(new Triangle(v011, v110, v010, materialIndex));

    // Левая грань (x = -s)
    m_Scene->Add(new Triangle(v001, v011, v010, materialIndex));
    m_Scene->Add(new Triangle(v001, v010, v000, materialIndex));

    // Правая грань (x = +s)
    m_Scene->Add(new Triangle(v110, v111, v101, materialIndex));
    m_Scene->Add(new Triangle(v110, v101, v100, materialIndex));

    // Задняя грань (z = -s)
    m_Scene->Add(new Triangle(v010, v110, v100, materialIndex));
    m_Scene->Add(new Triangle(v010, v100, v000, materialIndex));

    // Передняя грань (z = +s)
    m_Scene->Add(new Triangle(v101, v111, v011, materialIndex));
    m_Scene->Add(new Triangle(v101, v011, v001, materialIndex));
}
