#include "Application.h"

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
}

int Application::Run() const {
    return m_QtApplication->exec();
}

void Application::OnRender() {
    QElapsedTimer timer;
    timer.start();
    m_Renderer->Render();
    const qint64 elapsedMs = timer.elapsed();
    m_Window->UpdateRenderTime(elapsedMs);

    const auto imageData = m_Renderer->GetFinalImageData();
    m_Window->ShowImage(imageData);
}

void Application::OnUpdate(float deltaTime) {
}

void Application::OnCanvasResize(const int width, const int height) {
    if (m_Renderer) {
        m_Renderer->OnResize(width, height);
    }
}
