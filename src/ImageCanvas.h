#pragma once

#include <QWidget>


class ImageCanvas final : public QWidget {
    Q_OBJECT
public:
    explicit ImageCanvas(QWidget* parent = nullptr);
    void SetImage(const QImage& img);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    QImage m_Image;
};
