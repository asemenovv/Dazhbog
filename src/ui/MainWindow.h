#pragma once

#include <QLabel>
#include <QMainWindow>

#include "ImageCanvas.h"
#include "RenderSettingsWidget.h"
#include "../render/Renderer.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    enum class ButtonAction
    {
        Dump
    };

    using ResizeHandler = std::function<void(int,int)>;

    using ButtonHandler = std::function<void(ButtonAction)>;

    using RenderSettingsChangedHandler = std::function<void(Renderer::Settings)>;

    explicit MainWindow(const ResizeHandler &resizeHandler, QWidget* parent = nullptr);

    void SetButtonHandler(const ButtonHandler& handler);

    void UpdateFrame(uint32_t frameNo);

    void UpdateRenderTime(int64_t frameRenderTime, int64_t sceneRenderTime);

    void UpdateCameraLocation(const glm::vec3& position, const glm::vec3& direction);

    void ShowImage(const uint32_t* pixels) const;

    [[nodiscard]] glm::vec2 GetCanvasSize() const { return {m_Canvas->width(), m_Canvas->height()}; }

    void SetRenderSettingsChangedHandler(const RenderSettingsChangedHandler &handler);

    void SetRenderSettings(const Renderer::Settings& settings) const;

private slots:
    void resizeEvent(QResizeEvent* event) override;
    void onDumpClicked() const;
private:
    void setupUi();

    RenderSettingsWidget* m_RenderSettingsWidget = nullptr;

    ImageCanvas* m_Canvas = nullptr;
    QLabel* m_FrameLabel = nullptr;
    QLabel* m_FrameRenderTimeLabel = nullptr;
    QLabel* m_SceneRenderTimeLabel = nullptr;
    QLabel* m_CameraPositionLabel = nullptr;
    QLabel* m_CameraDirectionLabel = nullptr;
    ResizeHandler m_ResizeHandler;
    ButtonHandler m_ButtonHandler;
    RenderSettingsChangedHandler m_RenderSettingsChangedHandler;
};
