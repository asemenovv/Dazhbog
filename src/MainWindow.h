#pragma once

#include <QLabel>
#include <QMainWindow>

#include "ImageCanvas.h"
#include "render/Renderer.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    using ResizeHandler = std::function<void(int,int)>;

    explicit MainWindow(const ResizeHandler &resizeHandler, QWidget* parent = nullptr);

    void UpdateRenderTime(qint64 renderTime);

    void ShowImage(const uint32_t* pixels) const;

    glm::vec2 GetCanvasSize() const { return {m_Canvas->width(), m_Canvas->height()}; }

private slots:
    void resizeEvent(QResizeEvent* event) override;
    void onRenderClicked();
private:
    void setupUi();

    ImageCanvas* m_Canvas = nullptr;
    QLabel* m_RenderTimeLabel = nullptr;
    ResizeHandler m_ResizeHandler;
};
