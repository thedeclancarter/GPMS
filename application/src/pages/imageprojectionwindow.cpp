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

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_imageLabel);


    // QPixmap scaledPixmap = QPixmap::fromImage(image).scaled(1024, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // imageLabel->setPixmap(scaledPixmap);

    setStyleSheet("background-color: white;");
}

void ImageProjectionWindow::updateImage(const QImage &image)
{
    if (!image.isNull())
    {
        // Scale the image to fit the label while maintaining aspect ratio
        QPixmap pixmap = QPixmap::fromImage(image).scaled(
            m_imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        m_imageLabel->setPixmap(pixmap);
    }
    else
    {
        qDebug("Received null image in ImageProjectionWindow::updateImage");
    }
}

// Whiteout the current frame
void ImageProjectionWindow::clearImage(void)
{
    m_imageLabel->clear();
}
