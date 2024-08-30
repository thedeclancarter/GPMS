#include "takepicture.h"
#include "ui_takepicture.h"
#include <QMediaDevices>
#include <QImageCapture>
#include <QVBoxLayout>
#include <QPermissions>
#include <QDateTime>
#include <QStandardPaths>
#include <QUrl>


TakePicture::TakePicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TakePicture)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_viewfinder(nullptr)
    , m_captureButton(nullptr)
    , m_imageCapture(nullptr)
{
    ui->setupUi(this);
    init();
}

void TakePicture::setupCamera()
{
    m_camera = new QCamera(this);
    m_captureSession = new QMediaCaptureSession(this);
    m_viewfinder = new QVideoWidget(this);
    m_imageCapture = new QImageCapture(this);

    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_viewfinder);
    m_captureSession->setImageCapture(m_imageCapture);

    QVBoxLayout *layout = new QVBoxLayout(ui->webcamLayout);
    layout->addWidget(m_viewfinder);

    ui->takePictureButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #6F81CD;"
        "   border-radius: 15px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4A5A9F;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5D5D5D;"
        "}"
        );

    connect(ui->takePictureButton, &QPushButton::clicked, this, &TakePicture::captureImage);
    connect(m_imageCapture, &QImageCapture::imageCaptured, this, &TakePicture::handleImageCaptured);

    m_camera->start();
}

void TakePicture::captureImage()
{
    if (m_imageCapture->isReadyForCapture()) {
        m_imageCapture->capture();
    } else {
        qDebug() << "Image capture is not ready";
    }
}

void TakePicture::handleImageCaptured(int id, const QImage &preview)
{
    qDebug() << "Image captured with id:" << id;

    // You can process the image here if needed
    // For example, you might want to scale it down:
    QImage scaledImage = preview.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    emit imageCaptured(scaledImage);
    emit navigateToAcceptPicturePage();
}

// This is to get the permissions to use the camera
void TakePicture::init()
{
    #if QT_CONFIG(permissions)
        // Camera permission
        QCameraPermission cameraPermission;
        switch (qApp->checkPermission(cameraPermission)) {
        case Qt::PermissionStatus::Undetermined:
            qApp->requestPermission(cameraPermission, this, &TakePicture::init);
            return;
        case Qt::PermissionStatus::Denied:
            qWarning("Camera permission is not granted!");
            return;
        case Qt::PermissionStatus::Granted:
            break;
        }
    #endif

    // If we've reached here, permissions are granted (or not required)
    // So we can proceed with camera setup
    if (checkCameraAvailability()) {
            qDebug() << "Cameras found!";
        setupCamera();
    } else {
        qDebug() << "No camera available";
    }
}

void TakePicture::handleCameraError(QCamera::Error error)
{
    switch (error) {
    case QCamera::NoError:
        qDebug() << "No error occurred";
        break;
    case QCamera::CameraError:
        qDebug() << "General Camera error";
        break;
    default:
        qDebug() << "Unknown camera error:" << error;
    }
}

TakePicture::~TakePicture()
{
    delete ui;
    delete m_camera;
    delete m_captureSession;
    delete m_viewfinder;
    delete m_captureButton;
}

bool TakePicture::checkCameraAvailability()
{
    return QMediaDevices::videoInputs().count() > 0;
}
