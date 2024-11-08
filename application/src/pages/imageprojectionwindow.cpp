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
ImageProjectionWindow::ImageProjectionWindow()
    : QWindow()  // Changed from QWidget constructor // | Qt::FramelessWindowHint
    , m_loSensitivity(50) // Default sensitivity values
    , m_hiSensitivity(150)
    , m_state(projectionState::LOGO)
    , m_rainbowTimer(new QTimer(this))
    , m_frameCount(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
    // Remove window frame
    setFlags(Qt::Window | Qt::FramelessWindowHint);
    resize(FIXED_SIZE);

    // Create a container widget to hold the QLabel
    m_container = QWidget::createWindowContainer(this);
    m_container->setFixedSize(WIDTH, HEIGHT);

    setupUI();
    setProjectionState(projectionState::LOGO); // Initialize with LOGO state

    connect(m_rainbowTimer, &QTimer::timeout, this, &ImageProjectionWindow::updateRainbowEdges);
}

// attempt to show on projector
void ImageProjectionWindow::showOnProjector()
{
    QScreen* projectorScreen = findProjectorScreen();
    if (projectorScreen) {
        qDebug() << "Moving to projector screen:" << projectorScreen->name();
        moveToScreen(projectorScreen);
        m_isOnProjector = true;
    } else {
        qDebug() << "No projector found, showing on main screen";
        moveToScreen(QGuiApplication::primaryScreen());
        m_isOnProjector = false;
        setPosition(100, 100);
    }

    setupProjectorMode();
    show();
    requestActivate();
}

void ImageProjectionWindow::moveToScreen(QScreen* screen)
{
    if (!screen) return;

    // Set the target screen
    setScreen(screen);

    QRect screenGeometry = screen->geometry();

    // Calculate center position
    int x = screenGeometry.x() + (screenGeometry.width() - FIXED_SIZE.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - FIXED_SIZE.height()) / 2;

    // Move the window
    setPosition(x, y);

    // Also move the container widget to match
    if (m_container) {
        m_container->move(x, y);
    }
}

QScreen* ImageProjectionWindow::findProjectorScreen()
{
    const QList<QScreen*>& screens = QGuiApplication::screens();
    QScreen* primaryScreen = QGuiApplication::primaryScreen();

    if (screens.size() <= 1) {
        qDebug("Only found 1 screen");
        return nullptr;
    }

    // First try to find a screen that matches projector characteristics
    for (QScreen* screen : screens) {
        if (screen != primaryScreen) {
            // Check if this screen matches your projector's characteristics
            QSize screenSize = screen->size();
            if (screenSize == FIXED_SIZE) {
                qDebug("Found screen with the correct size");
                return screen; // Found a screen matching our target resolution
            }
            else{
                qDebug("Doesnt match characteristics of target screen");
            }
        }
    }

    // Fallback: return first non-primary screen
    for (QScreen* screen : screens) {
        if (screen != primaryScreen) {
            qDebug() << "Found a non primary screen: " << screen;
            return screen;
        }
    }

    return nullptr;
}

void ImageProjectionWindow::setupProjectorMode()
{
    if (m_isOnProjector) {
        // Settings specific to projector display
        setFlags(Qt::Window | Qt::FramelessWindowHint);
        // Ensure window stays on top when on projector
        setFlags(flags() | Qt::WindowStaysOnTopHint);
    } else {
        // Settings for regular window mode
        setFlags(Qt::Window | Qt::FramelessWindowHint);
    }
}

// Setup UI
void ImageProjectionWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(m_container);
    layout->setContentsMargins(0, 0, 0, 0);

    m_imageLabel = new QLabel(m_container);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setFixedSize(WIDTH, HEIGHT);
    layout->addWidget(m_imageLabel);

    m_container->setStyleSheet("background-color: white;");
}

// Setters
void ImageProjectionWindow::setStillFrame(const cv::Mat &mat)
{
    if (mat.empty()) {
        qDebug() << "Empty Mat provided to setStillFrame.";
        return;
    }

    m_updateEdgeDetectionFrame = true;
    m_stillFrame = mat.clone();
}

void ImageProjectionWindow::setFinalFrame(const cv::Mat &mat)
{
    if (mat.empty()) {
        qDebug() << "Empty Mat provided to setStillFrame.";
        return;
    }

    m_finalFrame = mat.clone();
    setProjectionState(m_state);
}

void ImageProjectionWindow::setSensitivity(int lo, int hi)
{
    m_loSensitivity = lo;
    m_hiSensitivity = hi;

    m_updateEdgeDetectionFrame = true;

    setProjectionState(projectionState::EDGE_DETECTION);
}

void ImageProjectionWindow::setTransformCorners(const std::array<cv::Point2f, 4>& transformCorners)
{
    m_transformCorners = transformCorners;
    m_updatePerspectiveMatrix = true;
    setProjectionState(projectionState::EDGE_DETECTION);
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
        QPixmap pix = m_imageLabel->pixmap(Qt::ReturnByValue);
        if (!pix.isNull()) {
            return pix.toImage();
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
    m_container->setStyleSheet("background-color: white;");
}

// Activate EDGE_DETECTION state
void ImageProjectionWindow::activateEdgeDetection()
{
    if (m_stillFrame.empty()) {
        qDebug() << "No still frame set for edge detection.";
        return;
    }

    m_isCalibrated = true;

    // Handle Edge Detection Caching
    if (m_updateEdgeDetectionFrame)
    {
        // Convert to grayscale
        cv::Mat grayMat;
        cv::cvtColor(m_stillFrame, grayMat, cv::COLOR_BGR2GRAY);

        // Apply Canny edge detection
        cv::Mat edges;
        cv::Canny(grayMat, edges, m_loSensitivity, m_hiSensitivity);

        // Convert edges to BGR for consistent display format
        cv::cvtColor(edges, m_edgeDetectionFrame, cv::COLOR_GRAY2BGR);
        m_updateEdgeDetectionFrame = false;
    }
    // else // XXXX
    // {
    //     qDebug() << "This helped Edge!";
    // }

    // Apply the edge warping
    cv::Mat warpedMat = applyPerspectiveTransform(m_edgeDetectionFrame);

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
        updateImage(warpedMat); // setting m_imageLabel to contain this image
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
    const uchar* matData = mat.data;

    if (mat.channels() == 3)
    {
        // Convert BGR to RGB directly into QImage without extra cv::Mat
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        matData = rgbMat.data;
        image = QImage(matData, rgbMat.cols, rgbMat.rows, static_cast<int>(rgbMat.step), QImage::Format_RGB888);
    }
    else if (mat.channels() == 4)
    {
        // Convert BGRA to RGBA directly
        cv::Mat rgbaMat;
        cv::cvtColor(mat, rgbaMat, cv::COLOR_BGRA2RGBA);
        matData = rgbaMat.data;
        image = QImage(matData, rgbaMat.cols, rgbaMat.rows, static_cast<int>(rgbaMat.step), QImage::Format_RGBA8888);
    }
    else if (mat.channels() == 1)
    {
        // Grayscale image
        image = QImage(matData, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
    }
    else
    {
        qDebug() << "Unsupported number of channels in Mat for updateImage: " << mat.channels();
        return;
    }

    if (!image.isNull())
    {
        // Since matData might go out of scope, we need to make a deep copy now
        image = image.copy();

        // Scale the image to fit the label while maintaining aspect ratio
        QPixmap pixmap = QPixmap::fromImage(image).scaled(
            m_imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        m_imageLabel->setPixmap(pixmap);
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

    QPixmap pixmap;

    // Check if scaling is necessary
    if (image.size() != m_imageLabel->size())
    {
        // Scale the image to fit the label while maintaining aspect ratio
        pixmap = QPixmap::fromImage(image).scaled(
            m_imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
    }
    else
    {
        pixmap = QPixmap::fromImage(image);
    }

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

    if (m_updatePerspectiveMatrix) {
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
        m_perspectiveMatrix = cv::getPerspectiveTransform(dstCorners.data(), srcCorners.data());
        m_updatePerspectiveMatrix = false;
    }
    // else // XXXX
    // {
    //     qDebug() << "This helped Transform!";
    // }

    const cv::Size outputSize(WIDTH, HEIGHT);

    // Apply the perspective transformation
    cv::Mat warped;
    cv::warpPerspective(mat, warped, m_perspectiveMatrix, outputSize);

    return warped;
}

void ImageProjectionWindow::updateRainbowEdges()
{
    if (m_stillFrame.empty()) {
        qDebug() << "No still frame set for rainbow edge detection.";
        return;
    }

    cv::Mat edges = m_edgeDetectionFrame;
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

ImageProjectionWindow::~ImageProjectionWindow()
{
    delete m_container;
}
