#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"
#include "imageprojectionwindow.h"

#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QtMultimedia/qcameraimagecapture.h>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QCameraInfo>


SensitivityPage::SensitivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
    , m_projectionWindow(nullptr)
    , m_imageLabel(nullptr)
    , lowerSlider(nullptr)
    , upperSlider(nullptr)
    , timer(nullptr)
    , m_camera(nullptr)
    , m_viewfinder(nullptr)
    , m_imageCapture(nullptr)
{
    ui->setupUi(this);
    init();

    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);

    connect(lowerSlider, &QSlider::valueChanged, this, &SensitivityPage::updateCannyEdgeDetection);
    connect(upperSlider, &QSlider::valueChanged, this, &SensitivityPage::updateCannyEdgeDetection);

    // Set up timer for continuous frame capture
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,  this, &SensitivityPage::captureImage);
    timer->start(33);
}

void SensitivityPage::captureAndProcessFrame()
{
    // cv::Mat frame;
    // if (capture.read(frame)) {
    //     cv::Mat edges;
    //     cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
    //     cv::Canny(edges, edges, lowerSlider->value(), upperSlider->value());

    //     // Convert edges to QImage and display
    //     QImage edgeImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8);
    //     m_imageLabel->setPixmap(QPixmap::fromImage(edgeImage).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // }
}

void SensitivityPage::init()
{
    if (checkCameraAvailability()) {
        qDebug() << "Cameras found!";
        initializeUI();
    } else {
        qDebug() << "No camera available";
        // You might want to show an error message in the UI here
    }
}

void SensitivityPage::captureImage()
{
    if (m_imageCapture->isReadyForCapture()) {
        qDebug() << "Image about to be taken";
        int id = m_imageCapture->capture();
        qDebug() << "Capture initiated with id:" << id;
    } else {
        qDebug() << "Image capture is not ready";
    }
}

void SensitivityPage::setupCamera()
{
    m_camera = new QCamera(this);
    m_viewfinder = new QCameraViewfinder(this);
    m_imageCapture = new QCameraImageCapture(m_camera);

    m_camera->setViewfinder(m_viewfinder);

    m_viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
    m_viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // connect(ui->takePictureButton, &QPushButton::clicked, this, &TakePicture::captureImage);
    // connect(m_imageCapture, &QCameraImageCapture::imageCaptured, this, &TakePicture::handleImageCaptured);


    // // Add these new connections for more detailed reporting
    // connect(m_imageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &TakePicture::handleReadyForCaptureChanged);
    // connect(m_imageCapture, static_cast<void(QCameraImageCapture::*)(int, QCameraImageCapture::Error, const QString &)>(&QCameraImageCapture::error),
    //         this, &TakePicture::handleCaptureError);
    // connect(m_imageCapture, &QCameraImageCapture::imageAvailable, this, &TakePicture::handleImageAvailable);
    // connect(m_imageCapture, &QCameraImageCapture::imageSaved, this, &TakePicture::handleImageSaved);

    // Set capture mode to CaptureToBuffer
    m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    m_camera->start();
}


void SensitivityPage::setProjectionWindow(ImageProjectionWindow *projectionWindow)
{
    m_projectionWindow = projectionWindow;
    qDebug("calling set projection window");

}

void SensitivityPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel());

    mainLayout->addWidget(createImageFrame(), 1);

    QFrame *sliderFrame = new QFrame(this);
    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderFrame);
    sliderFrame->setLayout(sliderLayout);

    lowerSlider = new QSlider(Qt::Horizontal, this);
    upperSlider = new QSlider(Qt::Horizontal, this);

    createSlider(lowerSlider);
    createSlider(upperSlider);

    sliderLayout->addWidget(lowerSlider);
    sliderLayout->addWidget(upperSlider);

    mainLayout->addWidget(sliderFrame);

    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);
}

QLabel* SensitivityPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Change the sensitivity of the outline", this);
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

QFrame* SensitivityPage::createImageFrame()
{
    setupCamera();

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

QSlider* SensitivityPage::createSlider(QSlider* slider)
{
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 300);  // Adjust range as needed
    slider->setValue(150);     // Default value
    slider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #999999;"
        "    height: 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
        "    margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
        "    border: 1px solid #5c5c5c;"
        "    width: 18px;"
        "    margin: -2px 0;"
        "    border-radius: 9px;"
        "}"
        );
    slider->setFixedHeight(40);
    return slider;
}

QHBoxLayout* SensitivityPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->rejectSensitivityButton, "LET'S TRY AGAINN!", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->acceptSensitivityButton, "THIS LOOKS GOODD!", "#6FCD6F"));
    return buttonLayout;
}

QPushButton* SensitivityPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    button->setText(text);
    button->setFixedSize(200, 50);
    button->setStyleSheet(QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: white;"
        "   border-radius: 25px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: darker(%1, 120%);"
        "}"
        "QPushButton:pressed {"
        "   background-color: darker(%1, 140%);"
        "}"
    ).arg(bgColor));
    return button;
}

SensitivityPage::~SensitivityPage()
{
    if (timer) {
        timer->stop();
    }
    delete ui;
}

void SensitivityPage::onAcceptButtonClicked()
{
    // if (!currentImage.isNull()) {
    //     ImageProjectionWindow *projectionWindow = new ImageProjectionWindow(currentImage);
    //     projectionWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure the window is deleted when closed
    //     projectionWindow->show();
    // }

    emit navigateToTextVisionPage();
}

bool SensitivityPage::checkCameraAvailability()
{
    return !QCameraInfo::availableCameras().isEmpty();
}

void SensitivityPage::updateCannyEdgeDetection()
{
    // QMutexLocker locker(&m_frameMutex);
    // if (!m_lastFrame.empty())
    // {
    //     cv::Mat edges;
    //     cv::cvtColor(m_lastFrame, edges, cv::COLOR_BGR2GRAY);
    //     cv::Canny(edges, edges, lowerSlider->value(), upperSlider->value());

    //     // Convert edges to QImage and display
    //     QImage edgeImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8);
    //     m_imageLabel->setPixmap(QPixmap::fromImage(edgeImage).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // }
}

