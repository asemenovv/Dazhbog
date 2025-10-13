#include "Application.h"

#include <QKeyEvent>

Application::Application(int argc, char *argv[]) {
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

    m_Renderer = std::make_unique<Renderer>(m_Window->GetCanvasSize());
    m_Camera = std::make_unique<Camera>(45.0, 0.1, 100.0, m_Window->GetCanvasSize());
}

int Application::Run() {
    m_QtApplication->installEventFilter(this);
    return m_QtApplication->exec();
}

void Application::OnRender() {
    QElapsedTimer timer;
    timer.start();
    m_Renderer->Render(m_Camera.get());
    m_RenderTimeMs = timer.elapsed();
    m_Window->UpdateRenderTime(m_RenderTimeMs);

    const auto imageData = m_Renderer->GetFinalImageData();
    m_Window->ShowImage(imageData);
}

void Application::OnUpdate(float deltaTime) {
}

void Application::OnCanvasResize(const int width, const int height) {
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
