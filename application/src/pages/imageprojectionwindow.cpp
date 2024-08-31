#include "imageprojectionwindow.h"
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>

ImageProjectionWindow::ImageProjectionWindow(const QImage &image, QWidget *parent)
    : QWidget(parent, Qt::Window) // | Qt::FramelessWindowHint
{
    setupUI(image);
    setFixedSize(1024, 600);
}

void ImageProjectionWindow::setupUI(const QImage &image)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);

    if (!image.isNull()) {
        // QScreen *screen = QGuiApplication::primaryScreen();
        QPixmap scaledPixmap = QPixmap::fromImage(image).scaled(1024, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
    }

    setStyleSheet("background-color: black;");
}
