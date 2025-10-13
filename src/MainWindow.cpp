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

void MainWindow::UpdateRenderTime(const int64_t renderTime) {
    m_RenderTimeLabel->setText(QString("Render time: %1 ms").arg(renderTime));
}

void MainWindow::ShowImage(const uint32_t *pixels) const {
    const auto width = m_Canvas->width();
    const auto height = m_Canvas->height();
    const QImage img(reinterpret_cast<const uchar*>(pixels), width, height,
                     width * 4, QImage::Format_RGBA8888);
    m_Canvas->SetImage(img.flipped());
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    m_ResizeHandler(m_Canvas->width(), m_Canvas->height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::onRenderClicked() {
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
