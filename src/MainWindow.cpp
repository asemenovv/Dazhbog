#include "MainWindow.h"

#include <qelapsedtimer.h>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    statusBar()->showMessage("Ready");

    m_Renderer = std::make_unique<Renderer>();
}

void MainWindow::onRenderClicked() const {
    const int width = m_Canvas->width();
    const int height = m_Canvas->height();
    m_Renderer->OnResize(width, height);

    QElapsedTimer timer;
    timer.start();
    m_Renderer->Render();

    const qint64 elapsedMs = timer.elapsed();
    m_RenderTimeLabel->setText(QString("Render time: %1 ms").arg(elapsedMs));

    const auto imageData = m_Renderer->GetFinalImageData();

    const QImage img(reinterpret_cast<const uchar*>(imageData), width, height,
        width * 4, QImage::Format_RGBA8888);
    m_Canvas->SetImage(img.flipped());
}

void MainWindow::setupUi() {
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    m_Canvas = new ImageCanvas(splitter);

    auto* rightPanel = new QFrame(splitter);
    rightPanel->setFrameShape(QFrame::NoFrame);
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(8, 8, 8, 8);
    rightLayout->setSpacing(8);

    auto* renderBtn = new QPushButton("Render", rightPanel);
    renderBtn->setObjectName("btnRender");

    m_RenderTimeLabel = new QLabel("Render time: –", rightPanel);

    rightLayout->addWidget(renderBtn);
    rightLayout->addWidget(m_RenderTimeLabel);
    rightLayout->addStretch(1);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    rightPanel->setMinimumWidth(180);
    rightPanel->setMaximumWidth(320);

    setCentralWidget(splitter);

    setWindowTitle("Dazhbog");

    connect(renderBtn, &QPushButton::clicked, this, &MainWindow::onRenderClicked);
}
