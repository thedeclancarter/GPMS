#include "imageprojectionwindow.h"
#include <QVBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QPixmap>
#include <QTransform>
#include <QPainter>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// Constructor
ImageProjectionWindow::ImageProjectionWindow(QWidget *parent)
    : QWidget(parent, Qt::Window) // | Qt::FramelessWindowHint
    , m_loSensitivity(50) // Default sensitivity values
    , m_hiSensitivity(150)
    , m_state(projectionState::LOGO)
    , m_rainbowTimer(new QTimer(this))
    , m_frameCount(0)
{
    setupUI();
    setFixedSize(WIDTH, HEIGHT);
    setProjectionState(projectionState::LOGO); // Initialize with LOGO state

    connect(m_rainbowTimer, &QTimer::timeout, this, &ImageProjectionWindow::updateRainbowEdges);
}

// Setup UI
void ImageProjectionWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setFixedSize(WIDTH, HEIGHT);
    layout->addWidget(m_imageLabel);

    setStyleSheet("background-color: white;");
}

// Setters
void ImageProjectionWindow::setStillFrame(const cv::Mat &mat)
{
    if (mat.empty()) {
        qDebug() << "Empty Mat provided to setStillFrame.";
        return;
    }

    m_stillFrame = mat.clone(); // Clone to ensure data integrity

    // Optionally update the current state image if needed
    setProjectionState(m_state);
}

void ImageProjectionWindow::setFinalFrame(const cv::Mat &mat)
{
    qDebug("In setFinalFRame");
    if (mat.empty()) {
        qDebug() << "Empty Mat provided to setStillFrame.";
        return;
    }

    m_finalFrame = mat.clone(); // Clone to ensure data integrity
    setProjectionState(m_state);
}

void ImageProjectionWindow::setSensitivity(int lo, int hi)
{
    m_loSensitivity = lo;
    m_hiSensitivity = hi;
    // Optionally update the current state image if edge detection is active
    if (m_state == projectionState::EDGE_DETECTION || m_state == projectionState::RAINBOW_EDGE) {
        setProjectionState(m_state);
    }
}

void ImageProjectionWindow::setTransformCorners(const std::array<cv::Point2f, 4>& transformCorners)
{
    m_transformCorners = transformCorners;
    // Optionally update the image projection if in IMAGE state
    if (m_state == projectionState::EDGE_DETECTION) {
        setProjectionState(m_state);
    }
}

void ImageProjectionWindow::setProjectionState(projectionState state)
{
    // Check if the current state is RAINBOW_EDGE and the new state is different
    if (m_state == projectionState::RAINBOW_EDGE && state != projectionState::RAINBOW_EDGE) {
        if (m_rainbowTimer->isActive()) {
            m_rainbowTimer->stop();
            qDebug() << "Rainbow timer stopped as state changed from RAINBOW_EDGE to another state.";
        } else {
            qDebug() << "Rainbow timer was not active when changing state from RAINBOW_EDGE.";
        }
    }

    // Update the current state
    m_state = state;

    // Activate the appropriate state
    switch (state)
    {
    case projectionState::LOGO:
        activateLogo();
        break;
    case projectionState::SCANNING:
        activateScanning();
        break;
    case projectionState::EDGE_DETECTION:
        activateEdgeDetection();
        break;
    case projectionState::RAINBOW_EDGE:
        activateRainbowEdge();
        break;
    case projectionState::IMAGE:
        qDebug("calling activate image");
        activateImage();
        break;
    default:
        qDebug() << "Unknown projection state:" << static_cast<int>(state);
        break;
    }
}

// Getters
bool ImageProjectionWindow::getIsCalibrated() const {
    return m_isCalibrated;
}

QImage ImageProjectionWindow::getCurrentImage() const
{
    if (m_imageLabel) {
        const QPixmap* pix = m_imageLabel->pixmap(); // Use the value-returning overload
        if (!pix->isNull()) {
            return pix->toImage();
        }
    }
    else{
        qDebug("In getCurrentImage, m_imageLabel is null");
    }
    return QImage(); // Return an empty QImage if there's no pixmap
}


// State Transitions

// Activate LOGO state
void ImageProjectionWindow::activateLogo(){
    // Load the image from the resource
    QImage logoImage(":/icons/projLogo.png");

    if (!logoImage.isNull())
    {
        updateImage(logoImage);
    }
    else
    {
        qDebug() << "Failed to load logo image from resource";
    }
}

// Activate SCANNING state (whiteout the current frame)
void ImageProjectionWindow::activateScanning()
{
    m_isCalibrated = false;
    m_imageLabel->clear();
    setStyleSheet("background-color: white;");
}

// Activate EDGE_DETECTION state
void ImageProjectionWindow::activateEdgeDetection()
{
    if (m_stillFrame.empty()) {
        qDebug() << "No still frame set for edge detection.";
        return;
    }

    m_isCalibrated = true;

    // Convert to grayscale
    cv::Mat grayMat;
    cv::cvtColor(m_stillFrame, grayMat, cv::COLOR_BGR2GRAY);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(grayMat, edges, m_loSensitivity, m_hiSensitivity);

    // Convert edges to BGR for consistent display format
    cv::Mat edgesBGR;
    cv::cvtColor(edges, edgesBGR, cv::COLOR_GRAY2BGR);

    // Apply the edge warping
    cv::Mat warpedMat = applyPerspectiveTransform(edgesBGR);

    // Update the QLabel with the edge-detected image
    updateImage(warpedMat);
}

// Activate RAINBOW_EDGE state
void ImageProjectionWindow::activateRainbowEdge()
{
    if (m_stillFrame.empty()) {
        qDebug() << "No still frame set for rainbow edge detection.";
        return;
    }

    // Start the timer to update rainbow edges periodically
    if (!m_rainbowTimer->isActive()) {
        m_frameCount = 0;          // Reset frame count
        m_rainbowTimer->start(100); // Update every 100 ms (adjust as needed)
    }
}

// Activate IMAGE state (apply perspective transform)
void ImageProjectionWindow::activateImage()
{
    if (m_finalFrame.empty()) {
        qDebug() << "No still frame set for image projection.";
        return;
    }

    // Apply perspective transform using the helper function
    cv::Mat warpedMat = applyPerspectiveTransform(m_finalFrame);

    if (!warpedMat.empty()) {
        updateImage(m_finalFrame); // setting m_imageLabel to contain this image
    }
    else {
        qDebug() << "Failed to apply perspective transform in activateImage().";
    }
}


// Helper function to update the QLabel with a new image
void ImageProjectionWindow::updateImage(const cv::Mat &mat)
{
    if (mat.empty())
    {
        qDebug() << "Received empty Mat in ImageProjectionWindow::updateImage";
        return;
    }

    QImage image;
    if (mat.channels() == 3)
    {
        // Convert BGR to RGB
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        image = QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, static_cast<int>(rgbMat.step), QImage::Format_RGB888).copy();
    }
    else if (mat.channels() == 4)
    {
        // Convert BGRA to RGBA
        cv::Mat rgbaMat;
        cv::cvtColor(mat, rgbaMat, cv::COLOR_BGRA2RGBA);
        image = QImage(rgbaMat.data, rgbaMat.cols, rgbaMat.rows, static_cast<int>(rgbaMat.step), QImage::Format_RGBA8888).copy();
    }
    else if (mat.channels() == 1)
    {
        // Grayscale image
        image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
    }
    else
    {
        qDebug() << "Unsupported number of channels in Mat for updateImage: " << mat.channels();
        return;
    }

    if (!image.isNull())
    {
        // Scale the image to fit the label while maintaining aspect ratio
        QPixmap pixmap = QPixmap::fromImage(image).scaled(
            m_imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        m_imageLabel->setPixmap(pixmap);

        qDebug() << "QImage created successfully. Size:" << image.size();
        qDebug() << "Scaled pixmap size:" << pixmap.size();
        qDebug() << "Label size:" << m_imageLabel->size();
        qDebug() << "Label geometry:" << m_imageLabel->geometry();
        qDebug() << "Label is visible:" << m_imageLabel->isVisible();

    }
    else
    {
        qDebug() << "Conversion from Mat to QImage resulted in a null image.";
    }
}

void ImageProjectionWindow::updateImage(const QImage &image)
{
    if (image.isNull())
    {
        qDebug() << "Received null QImage in ImageProjectionWindow::updateImage";
        return;
    }

    // Scale the image to fit the label while maintaining aspect ratio
    QPixmap pixmap = QPixmap::fromImage(image).scaled(
        m_imageLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_imageLabel->setPixmap(pixmap);
}

// Helper function to apply perspective transform to a cv::Mat and return a QImage
cv::Mat ImageProjectionWindow::applyPerspectiveTransform(const cv::Mat& mat)
{
    if (mat.empty()) {
        qDebug() << "Empty mat provided to applyPerspectiveTransform.";
        return cv::Mat();
    }

    if (m_transformCorners.size() != 4) {
        qDebug() << "Transform corners are not properly set.";
        return cv::Mat();
    }

    // Define source points (corners of the original image)
    const std::vector<cv::Point2f> srcCorners = {
        {0.0f, 0.0f},
        {static_cast<float>(WIDTH), 0.0f},
        {static_cast<float>(WIDTH), static_cast<float>(HEIGHT)},
        {0.0f, static_cast<float>(HEIGHT)}
    };

    // Destination points (transform corners)
    const std::array<cv::Point2f, 4> dstCorners = m_transformCorners;

    // Calculate the perspective transform matrix
    const cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(dstCorners.data(), srcCorners.data());

    const cv::Size outputSize(WIDTH, HEIGHT);

    // Apply the perspective transformation
    cv::Mat warped;
    cv::warpPerspective(mat, warped, perspectiveMatrix, outputSize);

    return warped;
}

void ImageProjectionWindow::updateRainbowEdges()
{
    if (m_stillFrame.empty()) {
        qDebug() << "No still frame set for rainbow edge detection.";
        return;
    }

    // Convert to grayscale
    cv::Mat grayMat;
    cv::cvtColor(m_stillFrame, grayMat, cv::COLOR_BGR2GRAY);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(grayMat, edges, m_loSensitivity, m_hiSensitivity);

    // Apply the rainbow effect to the edges
    cv::Mat rainbow_edges = cv::Mat::zeros(edges.size(), CV_8UC3);

    // Create a rainbow gradient (HSV color space)
    cv::Mat hue(edges.size(), CV_8UC1);
    for (int i = 0; i < edges.cols; i++) {
        hue.col(i) = static_cast<uchar>((i + m_frameCount * 5) % 180);
    }

    cv::Mat saturation = cv::Mat::ones(edges.size(), CV_8UC1) * 255;
    cv::Mat value = cv::Mat::ones(edges.size(), CV_8UC1) * 255;

    std::vector<cv::Mat> hsv_channels = { hue, saturation, value };
    cv::Mat hsv_rainbow;
    cv::merge(hsv_channels, hsv_rainbow);

    // Convert HSV to BGR
    cv::Mat rainbow;
    cv::cvtColor(hsv_rainbow, rainbow, cv::COLOR_HSV2BGR);

    // Apply the rainbow to the edges using vectorized operations
    rainbow.copyTo(rainbow_edges, edges);

    // Apply perspective transform using the helper function
    cv::Mat transformedMat = applyPerspectiveTransform(rainbow_edges);

    if (!transformedMat.empty()) {
        updateImage(transformedMat); // Use the cv::Mat version
    }
    else {
        qDebug() << "Failed to apply perspective transform in updateRainbowEdges().";
    }

    m_frameCount++; // Increment frame count for animation
}
