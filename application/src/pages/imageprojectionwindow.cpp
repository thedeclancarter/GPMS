#include "imageprojectionwindow.h"
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QPainter>

ImageProjectionWindow::ImageProjectionWindow(QWidget *parent)
    : QWidget(parent, Qt::Window) // | Qt::FramelessWindowHint
    ,m_videoWidget(nullptr)
    ,m_mediaPlayer(nullptr)
{
    setupUI();
    setFixedSize(1024, 600);
}

void ImageProjectionWindow::clearToWhite()
{
    // Stop any playing video
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }

    // Hide video widget and image label
    if (m_videoWidget) {
        m_videoWidget->hide();
    }
    if (m_imageLabel) {
        m_imageLabel->hide();
    }

    // Set white background
    setStyleSheet("background-color: white;");

    // Repaint the window
    update();
}

void ImageProjectionWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_imageLabel);


    setStyleSheet("background-color: white;");
}


void ImageProjectionWindow::setupVideoPlayer()
{
    if (!m_videoWidget) {
        m_videoWidget = new QVideoWidget(this);
        m_mediaPlayer = new QMediaPlayer(this);
        m_mediaPlayer->setVideoOutput(m_videoWidget);

        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
        if (layout) {
            layout->addWidget(m_videoWidget);
        }

        m_videoWidget->hide(); // Hide initially
    }
}


void ImageProjectionWindow::playVideoFromResource(const QString &filePath)
{
    setupVideoPlayer();

    m_imageLabel->hide();
    m_videoWidget->show();

    m_mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));
    m_mediaPlayer->play();
}


void ImageProjectionWindow::playInitialVideo()
{
    playVideoFromResource("qrc:/videos/background.mov");
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
