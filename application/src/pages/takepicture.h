#ifndef TAKEPICTURE_H
#define TAKEPICTURE_H

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
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
    QCameraViewfinder *m_viewfinder;
    QPushButton *m_captureButton;
    QCameraImageCapture *m_imageCapture;

    void init();
    bool checkCameraAvailability();
    void setupCamera();
    void handleCameraError(QCamera::Error error);
    void handleReadyForCaptureChanged(bool ready);
    void handleCaptureError(int id, QCameraImageCapture::Error error, const QString &errorString);
    void handleImageSaved(int id, const QString &fileName);

    void handleImageAvailable(int id, const QVideoFrame &buffer);
    void captureImage();
    void handleImageCaptured(int id, const QImage &preview);
    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createCameraFrame();
    QPushButton* createSubmitButton();
    void initializeCamera();
};

#endif // TAKEPICTURE_H
