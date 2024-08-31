#ifndef TAKEPICTURE_H
#define TAKEPICTURE_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QLabel>
#include <QFrame>
#include <QPushButton>

namespace Ui {
class TakePicture;
}

class TakePicture : public QWidget
{
    Q_OBJECT

public:
    explicit TakePicture(QWidget *parent = nullptr);
    ~TakePicture();

signals:
    void navigateToAcceptPicturePage();
    void imageCaptured(const QImage &image);

private:
    Ui::TakePicture *ui;

    QCamera *m_camera;
    QMediaCaptureSession *m_captureSession;
    QVideoWidget *m_viewfinder;
    QPushButton *m_captureButton;
    QImageCapture *m_imageCapture;

    void init();
    bool checkCameraAvailability();
    void setupCamera();
    void handleCameraError(QCamera::Error error);
    void captureImage();
    void handleImageCaptured(int id, const QImage &preview);

    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createCameraFrame();
    QPushButton* createSubmitButton();
    void initializeCamera();

};

#endif // TAKEPICTURE_H
