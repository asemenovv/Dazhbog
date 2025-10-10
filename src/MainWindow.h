#pragma once

#include <QLabel>
#include <QMainWindow>

#include "ImageCanvas.h"
#include "render/Renderer.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onRenderClicked() const;
private:
    void setupUi();

    ImageCanvas* m_Canvas = nullptr;
    QLabel* m_RenderTimeLabel = nullptr;
    std::unique_ptr<Renderer> m_Renderer;
};
