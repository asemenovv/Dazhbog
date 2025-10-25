#include "ImageCanvas.h"

#include <QPainter>

ImageCanvas::ImageCanvas(QWidget *parent): QWidget(parent) {
    setMinimumSize(200, 150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ImageCanvas::SetImage(const QImage &img) {
    m_Image = img;
    update();
}

void ImageCanvas::paintEvent(QPaintEvent *paint_event) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    if (m_Image.isNull()) return;

    // Вписываем с сохранением пропорций, без сглаживания можно выключить
    const QSize targetSize = m_Image.size().scaled(size(), Qt::KeepAspectRatio);
    const QPoint topLeft = QPoint(
        (width() - targetSize.width())/2,
        (height() - targetSize.height())/2
    );
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawImage(QRect(topLeft, targetSize), m_Image);
}
