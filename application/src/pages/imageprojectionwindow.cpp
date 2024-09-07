#include "imageprojectionwindow.h"
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>

ImageProjectionWindow::ImageProjectionWindow(QWidget *parent)
    : QWidget(parent, Qt::Window) // | Qt::FramelessWindowHint
{
    setupUI();
    setFixedSize(1024, 600);
}

void ImageProjectionWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);


    // QPixmap scaledPixmap = QPixmap::fromImage(image).scaled(1024, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // imageLabel->setPixmap(scaledPixmap);

    setStyleSheet("background-color: white;");
}
