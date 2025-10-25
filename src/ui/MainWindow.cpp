#include "MainWindow.h"

#include <qelapsedtimer.h>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QResizeEvent>


MainWindow::MainWindow(const ResizeHandler &resizeHandler, QWidget* parent) : QMainWindow(parent), m_ResizeHandler(resizeHandler) {
    setupUi();
    statusBar()->showMessage("Ready");
}

void MainWindow::SetButtonHandler(const ButtonHandler& handler)
{
    m_ButtonHandler = handler;
}

void MainWindow::UpdateRenderTime(const int64_t frameRenderTime, const int64_t sceneRenderTime) {
    m_FrameRenderTimeLabel->setText(QString("Frame Render Time: %1 ms").arg(frameRenderTime));
    m_SceneRenderTimeLabel->setText(QString("Scene Render Time: %1 ms").arg(sceneRenderTime));
}

void MainWindow::UpdateFrame(const uint32_t frameNo) {
    m_FrameLabel->setText(QString("Current Frame: %1").arg(frameNo));
}

void MainWindow::UpdateCameraLocation(const glm::vec3& position, const glm::vec3& direction) {
    m_CameraPositionLabel->setText(QString("Camera Position: (%1, %2, %3)")
        .arg(position.x,6, 'f', 2)
        .arg(position.y,6, 'f', 2)
        .arg(position.z,6, 'f', 2));
    m_CameraDirectionLabel->setText(QString("Camera Direction: (%1, %2, %3)")
        .arg(direction.x, 6, 'f', 2)
        .arg(direction.y, 6, 'f', 2)
        .arg(direction.z, 6, 'f', 2));
}

void MainWindow::ShowImage(const uint32_t *pixels) const {
    const auto width = m_Canvas->width();
    const auto height = m_Canvas->height();
    const QImage img(reinterpret_cast<const uchar*>(pixels), width, height,
                     width * 4, QImage::Format_RGBA8888);
    m_Canvas->SetImage(img.flipped());
}

void MainWindow::SetRenderSettingsChangedHandler(const RenderSettingsChangedHandler &handler) {
    m_RenderSettingsChangedHandler = handler;
}

void MainWindow::SetRenderSettings(const Renderer::Settings &settings) const {
    m_RenderSettingsWidget->SetSettings(settings);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    m_ResizeHandler(m_Canvas->width(), m_Canvas->height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::onDumpClicked() const
{
    if (m_ButtonHandler)
    {
        m_ButtonHandler(ButtonAction::Dump);
    }
}

void MainWindow::setupUi() {
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    m_Canvas = new ImageCanvas();;
    auto* rightPanel = new QFrame();

    splitter->addWidget(m_Canvas);
    splitter->addWidget(rightPanel);

    QList<int> sizes;
    sizes << 800 << 320; // левая область 800px, правая 320px
    splitter->setSizes(sizes);

    rightPanel->setFrameShape(QFrame::NoFrame);
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(8, 8, 8, 8);
    rightLayout->setSpacing(8);

    m_RenderSettingsWidget = new RenderSettingsWidget(rightPanel);

    connect(m_RenderSettingsWidget, &RenderSettingsWidget::settingsChanged,
            this, [&](const Renderer::Settings& settings){
                if (m_RenderSettingsChangedHandler) {
                    m_RenderSettingsChangedHandler(settings);
                }
            });

    auto* dumpBtn = new QPushButton("Dump", rightPanel);
    dumpBtn->setObjectName("btnDump");

    m_FrameLabel = new QLabel("Current Frame: -", rightPanel);
    m_FrameRenderTimeLabel = new QLabel("Frame Render time: –", rightPanel);
    m_SceneRenderTimeLabel = new QLabel("Scene Render time: –", rightPanel);
    m_CameraPositionLabel = new QLabel("Camera Position: –", rightPanel);
    m_CameraDirectionLabel = new QLabel("Camera Direction: –", rightPanel);

    rightLayout->addWidget(m_RenderSettingsWidget);
    rightLayout->addWidget(dumpBtn);
    rightLayout->addWidget(m_FrameLabel);
    rightLayout->addWidget(m_FrameRenderTimeLabel);
    rightLayout->addWidget(m_SceneRenderTimeLabel);
    rightLayout->addWidget(m_CameraPositionLabel);
    rightLayout->addWidget(m_CameraDirectionLabel);
    rightLayout->addStretch(1);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    rightPanel->setMinimumWidth(300);
    rightPanel->setMaximumWidth(640);

    setCentralWidget(splitter);

    setWindowTitle("Dazhbog");

    connect(dumpBtn, &QPushButton::clicked, this, &MainWindow::onDumpClicked);
}
