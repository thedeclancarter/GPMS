#ifndef IMAGEPROJECTIONWINDOW_H
#define IMAGEPROJECTIONWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QVideoWidget>
#include <QMediaPlayer>

class ImageProjectionWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImageProjectionWindow(QWidget *parent = nullptr);
    void updateImage(const QImage &image);
    void playVideoFromResource(const QString &filePath);
    void playInitialVideo();
    void clearToWhite();

private:
    QLabel *m_imageLabel;
    QVideoWidget *m_videoWidget;
    QMediaPlayer *m_mediaPlayer;

    void setupVideoPlayer();

    void setupUI();
};

#endif // IMAGEPROJECTIONWINDOW_H
