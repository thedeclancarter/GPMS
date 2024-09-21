#include "takepicture.h"
#include "ui_takepicture.h"
#include <QCameraInfo>
#include <QVBoxLayout>
#include <QDateTime>
#include <QStandardPaths>
#include <QUrl>
#include <QLabel>
#include <QFrame>

TakePicture::TakePicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TakePicture)
    , m_camera(nullptr)
    , m_viewfinder(nullptr)
    , m_imageCapture(nullptr)
{
    ui->setupUi(this);
    init();
}

void TakePicture::init()
{
    // Qt5 doesn't have the QPermission system, so we remove the permission check

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
    QLabel *titleLabel = new QLabel("Take A Picture Of Your Structure", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"
        "border-radius: 20px;"
        "padding: 15px 20px;"
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
    cameraLayout->setContentsMargins(0, 0, 0, 0); // Optional: to avoid unwanted margins
    if (m_viewfinder) {
        cameraLayout->addWidget(m_viewfinder);
        m_viewfinder->setStyleSheet("border-radius: 8px;");
    } else {
        qDebug() << "Viewfinder is null!";
    }

    cameraFrame->setLayout(cameraLayout); // Ensure layout is set for the frame

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
    m_viewfinder = new QCameraViewfinder(this);
    m_imageCapture = new QCameraImageCapture(m_camera);

    m_camera->setViewfinder(m_viewfinder);

    m_viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
    m_viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    connect(ui->takePictureButton, &QPushButton::clicked, this, &TakePicture::captureImage);
    connect(m_imageCapture, &QCameraImageCapture::imageCaptured, this, &TakePicture::handleImageCaptured);


    // Add these new connections for more detailed reporting
    connect(m_imageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &TakePicture::handleReadyForCaptureChanged);
    connect(m_imageCapture, static_cast<void(QCameraImageCapture::*)(int, QCameraImageCapture::Error, const QString &)>(&QCameraImageCapture::error),
            this, &TakePicture::handleCaptureError);
    connect(m_imageCapture, &QCameraImageCapture::imageAvailable, this, &TakePicture::handleImageAvailable);
    connect(m_imageCapture, &QCameraImageCapture::imageSaved, this, &TakePicture::handleImageSaved);

    // Set capture mode to CaptureToBuffer
    m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);



    // // Add these new connections for more detailed error reporting
    // connect(m_imageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &TakePicture::handleReadyForCaptureChanged);
    // connect(m_imageCapture, static_cast<void(QCameraImageCapture::*)(int, QCameraImageCapture::Error, const QString &)>(&QCameraImageCapture::error),
    //         this, &TakePicture::handleCaptureError);

    m_camera->start();
}

void TakePicture::captureImage()
{
    if (m_imageCapture->isReadyForCapture()) {
        qDebug() << "Image about to be taken";
        int id = m_imageCapture->capture();
        qDebug() << "Capture initiated with id:" << id;
    } else {
        qDebug() << "Image capture is not ready";
    }
}

void TakePicture::handleImageCaptured(int id, const QImage &preview)
{
    qDebug() << "Image captured with id:" << id;

    if (preview.isNull()) {
        qDebug() << "Captured image preview is null!";
        return;
    }

    QImage scaledImage = preview.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit imageCaptured(scaledImage);
    emit navigateToAcceptPicturePage();
}

void TakePicture::handleImageAvailable(int id, const QVideoFrame &buffer)
{
    qDebug() << "Image available with id:" << id;
    if (buffer.isValid()) {
        QImage image = buffer.image();
        if (!image.isNull()) {
            handleImageCaptured(id, image);
        } else {
            qDebug() << "Converted image is null";
        }
    } else {
        qDebug() << "Video frame buffer is not valid";
    }
}

void TakePicture::handleImageSaved(int id, const QString &fileName)
{
    qDebug() << "Image saved with id:" << id << "to file:" << fileName;
}

bool TakePicture::checkCameraAvailability()
{
    return !QCameraInfo::availableCameras().isEmpty();
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

void TakePicture::handleReadyForCaptureChanged(bool ready)
{
    qDebug() << "Ready for capture changed:" << ready;
}

void TakePicture::handleCaptureError(int id, QCameraImageCapture::Error error, const QString &errorString)
{
    qDebug() << "Capture error for id" << id << ":" << errorString;
}


TakePicture::~TakePicture()
{
    delete ui;
    delete m_camera;
    delete m_viewfinder;
    delete m_imageCapture;
}
