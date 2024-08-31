#include "takepicture.h"
#include "ui_takepicture.h"
#include <QMediaDevices>
#include <QImageCapture>
#include <QVBoxLayout>
#include <QPermissions>
#include <QDateTime>
#include <QStandardPaths>
#include <QUrl>
#include <QLabel>
#include <QFrame>

TakePicture::TakePicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TakePicture)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_viewfinder(nullptr)
    , m_imageCapture(nullptr)
{
    ui->setupUi(this);
    init();

}


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
    // So we can proceed with camera setup and UI initialization
    if (checkCameraAvailability()) {
        qDebug() << "Cameras found!";
        setupCamera();
        initializeUI();
    } else {
        qDebug() << "No camera available";
        // You might want to show an error message in the UI here
    }
}

void TakePicture::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    mainLayout->addWidget(createTitleLabel());
    mainLayout->addSpacing(10);
    mainLayout->addWidget(createCameraFrame());
    mainLayout->addSpacing(10);
    mainLayout->addWidget(createSubmitButton(), 0, Qt::AlignHCenter);

    setLayout(mainLayout);
}

QLabel* TakePicture::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Take a picture of your structure", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"  // Same as webcam frame background
        "border-radius: 20px;"
        "padding: 15px 20px;"  // Vertical and horizontal padding
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    return titleLabel;
}

QFrame* TakePicture::createCameraFrame()
{
    QFrame *cameraFrame = new QFrame(this);
    cameraFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    cameraFrame->setLineWidth(2);
    cameraFrame->setStyleSheet("border-radius: 10px; background-color: #2E2E2E;");

    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraFrame);
    if (m_viewfinder) {
        cameraLayout->addWidget(m_viewfinder);
        m_viewfinder->setStyleSheet("border-radius: 8px;");
    } else {
        qDebug() << "Viewfinder is null!";
    }

    return cameraFrame;
}

QPushButton* TakePicture::createSubmitButton()
{
    ui->takePictureButton->setText("+ SUBMIT");
    ui->takePictureButton->setFixedSize(120, 40);
    ui->takePictureButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #6F81CD;"
        "   color: white;"
        "   border-radius: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4A5A9F;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5D5D5D;"
        "}"
        );
    return ui->takePictureButton;
}

void TakePicture::initializeCamera()
{
    if (checkCameraAvailability()) {
        setupCamera();
    } else {
        qDebug() << "No camera available";
    }
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

    m_viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
    m_viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    QImage scaledImage = preview.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit imageCaptured(scaledImage);
    emit navigateToAcceptPicturePage();
}

bool TakePicture::checkCameraAvailability()
{
    return QMediaDevices::videoInputs().count() > 0;
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
    delete m_imageCapture;
}
