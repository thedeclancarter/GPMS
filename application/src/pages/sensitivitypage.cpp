#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"
#include "imageprojectionwindow.h"

#include <QGraphicsScene>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QCameraInfo>


SensitivityPage::SensitivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
    , m_projectionWindow(nullptr)
    , m_camera(nullptr)
    , m_viewfinder(nullptr)
{
    ui->setupUi(this);
    init();

    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);

    connect(lowerSlider, &QSlider::valueChanged, this, [this]() {
        applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
    });

    connect(upperSlider, &QSlider::valueChanged, this, [this]() {
        applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
    });

    // Set up timer for continuous frame capture
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SensitivityPage::captureAndProcessFrame);
    timer->start(33);
}

void SensitivityPage::captureAndProcessFrame()
{
    cv::Mat frame;
    if (capture.read(frame)) {
        cv::Mat edges;
        cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
        cv::Canny(edges, edges, lowerSlider->value(), upperSlider->value());

        // Convert edges to QImage and display
        QImage edgeImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8);
        m_imageLabel->setPixmap(QPixmap::fromImage(edgeImage).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void SensitivityPage::init()
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

bool SensitivityPage::checkCameraAvailability()
{
    return !QCameraInfo::availableCameras().isEmpty();
}

void SensitivityPage::setupCamera()
{
    m_camera = new QCamera(this);
    m_viewfinder = new QCameraViewfinder(this);

    m_camera->setViewfinder(m_viewfinder);

    m_viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
    m_viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_camera->start();
}


void SensitivityPage::setProjectionWindow(ImageProjectionWindow *projectionWindow)
{
    // m_projectionWindow = projectionWindow;
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
    buttonLayout->addWidget(styleButton(ui->rejectSensitivityButton, "LET'S TRY AGAIN!", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->acceptSensitivityButton, "THIS LOOKS GOOD!", "#6FCD6F"));
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
    if (capture.isOpened()) {
        capture.release();
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

void SensitivityPage::applyCannyEdgeDetection(int lowerThreshold, int upperThreshold)
{
    if (currentImage.isNull()) {
        qDebug("No image received in applyCannyEdgeDetection");
        return; // No image to process
    }

    // Convert QImage to cv::Mat
    cv::Mat mat = cv::Mat(currentImage.height(), currentImage.width(), CV_8UC4, const_cast<uchar*>(currentImage.bits()), currentImage.bytesPerLine()).clone();

    // Convert the image to grayscale
    cv::Mat grayMat;
    cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(grayMat, edges, lowerThreshold, upperThreshold);

    // Convert edges (cv::Mat) back to QImage
    QImage edgeImage = QImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8).copy();

    // Display the processed image
    QPixmap pixmap = QPixmap::fromImage(edgeImage);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setSceneRect(pixmap.rect());
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

// void SensitivityPage::setAcceptedImage(const QImage &image)
// {
//     currentImage = image;

//     // Display the image
//     QPixmap pixmap = QPixmap::fromImage(currentImage);
//     QGraphicsScene *scene = new QGraphicsScene(this);
//     scene->addPixmap(pixmap);
//     ui->graphicsView->setScene(scene);
//     ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//     ui->graphicsView->setSceneRect(pixmap.rect());
//     ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

//     // Apply Canny edge detection with default values
//     applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
// }
