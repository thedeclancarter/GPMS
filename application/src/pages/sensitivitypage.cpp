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
    , m_lastFrame(nullptr)
{
    ui->setupUi(this);
    init();

    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);
    connect(ui->rejectSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onRejectButtonClicked);

    // Set up timer for continuous frame capture
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SensitivityPage::captureAndProcessFrame);
    timer->start(1000);

    // Connect camera frame signals
    connect(m_imageCapture, &QCameraImageCapture::imageAvailable,
            this, &SensitivityPage::processFrame);
}

void SensitivityPage::processFrame(int id, const QVideoFrame &frame)
{
    Q_UNUSED(id);

    if (frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);

        // Get the frame format
        QVideoFrame::PixelFormat pixelFormat = cloneFrame.pixelFormat();

        // Convert to OpenCV Mat
        cv::Mat mat;
        if (pixelFormat == QVideoFrame::Format_Jpeg) {
            // Handle JPEG-encoded frame
            std::vector<uchar> buffer(cloneFrame.bits(), cloneFrame.bits() + cloneFrame.mappedBytes());
            mat = cv::imdecode(buffer, cv::IMREAD_COLOR);
            if (mat.empty()) {
                qDebug() << "Failed to decode JPEG frame";
                cloneFrame.unmap();
                return;
            }
        } else {
            switch (pixelFormat) {
            case QVideoFrame::Format_RGB32:
            case QVideoFrame::Format_ARGB32:
                mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC4, cloneFrame.bits(), cloneFrame.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
                break;
            case QVideoFrame::Format_RGB24:
                mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC3, cloneFrame.bits(), cloneFrame.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
                break;
            default:
                qDebug() << "Unsupported pixel format:" << pixelFormat;
                cloneFrame.unmap();
                return;
            }
        }
        mat = mat.clone();

        // Convert to grayscale
        cv::Mat grayMat;
        cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);

        // Apply Canny edge detection
        cv::Mat edges;
        cv::Canny(grayMat, edges, lowerSlider->value(), upperSlider->value());

        // Convert back to QImage
        QImage edgeImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8);
        edgeImage = edgeImage.copy(); // Create a deep copy of the image data

        updateDisplays(edgeImage);

        cloneFrame.unmap();
    } else {
        qDebug() << "Received invalid frame";
    }
}

void SensitivityPage::updateDisplays(const QImage &image)
{
    // Update local display
    m_imageLabel->setPixmap(QPixmap::fromImage(image).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Update ImageProjectionWindow if it exists
    if (m_projectionWindow) {
        m_projectionWindow->updateImage(image);
    }
}

void SensitivityPage::captureAndProcessFrame()
{
    if (m_imageCapture->isReadyForCapture()) {
        m_imageCapture->capture();
    }
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
    if (projectionWindow){
        m_projectionWindow = projectionWindow;
        qDebug("Setting projection window");
    }
    else{
        qDebug("Projection window is null");
    }

}

void SensitivityPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel(), 0, Qt::AlignHCenter);

    mainLayout->addWidget(createImageFrame(), 1, Qt::AlignHCenter);

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
    QLabel *titleLabel = new QLabel("Change The Sensitivity of the Outline", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"
        "border-radius: 20px;"
        "padding: 15px 20px;"
        );
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    titleLabel->setFixedWidth(800);
    titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    return titleLabel;
}

QFrame* SensitivityPage::createImageFrame()
{
    setupCamera();
    QFrame *cameraFrame = new QFrame(this);
    cameraFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    cameraFrame->setLineWidth(2);
    cameraFrame->setStyleSheet("border-radius: 10px; background-color: #2E2E2E;");
    cameraFrame->setFixedWidth(800);


    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraFrame);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cameraLayout->addWidget(m_imageLabel);
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
    buttonLayout->addWidget(styleButton(ui->rejectSensitivityButton, "LET'S TRY AGAIN!", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->acceptSensitivityButton, "THIS LOOKS GOOD!", "#6FCD6F"));
    return buttonLayout;
}

QPushButton* SensitivityPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    // Compute the darker color by applying a 30% reduction
    QString darkerColor;
    if (bgColor == "#CD6F6F") {
        darkerColor = "#8B4D4D";  // 30% darker color for red
    } else if (bgColor == "#6FCD6F") {
        darkerColor = "#4B8A4B";  // 30% darker color for green
    } else {
        darkerColor = "#4A5A9F";  // Default color (if none of the above match)
    }

    button->setText(text);
    button->setFixedSize(200, 50);
    button->setStyleSheet(QString(
                              "QPushButton {"
                              "   background-color: %1;"  // Original color
                              "   color: white;"
                              "   border-radius: 25px;"
                              "   font-weight: bold;"
                              "   font-size: 16px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: %2;"  // 30% darker color for hover
                              "}"
                              "QPushButton:pressed {"
                              "   background-color: %3;"  // 30% darker color for pressed
                              "}"
                              ).arg(bgColor).arg(darkerColor).arg(darkerColor));

    // Set the hand cursor when hovering over the button
    button->setCursor(Qt::PointingHandCursor);
    return button;
}


SensitivityPage::~SensitivityPage()
{
    if (timer) {
        timer->stop();
    }
    delete ui;
}

void SensitivityPage::onRejectButtonClicked()
{
    // if (!currentImage.isNull()) {
    //     ImageProjectionWindow *projectionWindow = new ImageProjectionWindow(currentImage);
    //     projectionWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure the window is deleted when closed
    //     projectionWindow->show();
    // }

    emit navigateToCalibrationPage();
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

