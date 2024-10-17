// calibrationPage.cpp

#include "calibrationPage.h"
#include "ui_calibrationPage.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

// OpenCV includes
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Constants for frame dimensions
static constexpr int WIDTH = 1280;
static constexpr int HEIGHT = 720;

// Define the size of the smaller display area
static constexpr int DISPLAY_WIDTH = 800;
static constexpr int DISPLAY_HEIGHT = 450;

// Constructor
CalibrationPage::CalibrationPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::CalibrationPage),
    m_projectionWindow(projectionWindow),
    timer(new QTimer(this)),
    resetMode(true),
    pointsSelected(false),
    dragging(false),
    selectedCorner(-1),
    mouseX(-1),
    mouseY(-1),
    stillFrameCaptured(false),
    pointsChanged(false) // Initialize the flag
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);

    // Connect the timer to capture frames
    connect(timer, &QTimer::timeout, this, &CalibrationPage::captureFrame);

    // Connect the complete button to navigate to the sensitivity page
    connect(ui->completeButton, &QPushButton::clicked, this, &CalibrationPage::onCompleteButtonClicked);

    // **Connect the rejectCalibrationButton to resetPoints()**
    connect(ui->rejectCalibrationButton, &QPushButton::clicked, this, &CalibrationPage::resetPoints);


    // Start the camera
    startCamera();
}

// Destructor
CalibrationPage::~CalibrationPage()
{
    stopCamera();
    delete ui;
}

// Start the camera and begin frame capture
void CalibrationPage::startCamera()
{
    if (!cap.isOpened()) { // Prevent reopening if already open
        cap.open(0);
        if (!cap.isOpened()) {
            qDebug() << "Error: Could not open camera.";
            return;
        }
        cap.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
    }
    timer->start(30); // Adjust the interval as needed (e.g., ~30 FPS)
}

// Stop the camera and halt frame capture
void CalibrationPage::stopCamera()
{
    if (cap.isOpened()) {
        cap.release();
    }
    timer->stop();
}

// Capture a frame from the camera
void CalibrationPage::captureFrame()
{
    if (!stillFrameCaptured) {
        cap >> frame;
        if (frame.empty()) {
            qDebug() << "Error: Could not capture frame.";
            return;
        }

        processFrame(); // Process the captured frame

        update();
    }
}

// Process the captured frame
void CalibrationPage::processFrame()
{
    if (!stillFrameCaptured) {
        // Live frame processing
        cv::Mat displayFrame = frame.clone();

        // Draw selected points
        for (int i = 0; i < numSelectedPoints; ++i) {
            cv::circle(displayFrame, selectedPoints[i], 5, cv::Scalar(0, 255, 0), -1);
        }

        // Draw ROI if 4 points are selected
        if (numSelectedPoints == 4) {
            drawROI(displayFrame, selectedPoints);
        }

        if (dragging && selectedCorner != -1) {
            // Draw magnifying glass
            drawMagnifyingGlass(frame, displayFrame, mouseX, mouseY);
        }

        // Scale down the displayFrame to DISPLAY_WIDTH x DISPLAY_HEIGHT
        cv::Mat scaledFrame;
        cv::resize(displayFrame, scaledFrame, cv::Size(DISPLAY_WIDTH, DISPLAY_HEIGHT), 0, 0, cv::INTER_LINEAR);

        // Convert scaledFrame to QImage
        cv::Mat rgbFrame;
        cv::cvtColor(scaledFrame, rgbFrame, cv::COLOR_BGR2RGB);
        qimg = QImage(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888).copy();

        // If four points are selected and a still frame hasn't been captured yet, capture it
        if (numSelectedPoints == 4 && !stillFrameCaptured) {
            stillFrame = frame.clone();
            stillFrameCaptured = true;
            timer->stop(); // Stop capturing frames
            updateProjectionWindow();
            updateDisplayWithStillFrame(); // Update display with the still frame
        }

    } else {
        // Still frame handling
        if (pointsChanged) {
            updateDisplayWithStillFrame();
            pointsChanged = false; // Reset the flag
        }
    }
}

// Update the projection window based on the selected points
void CalibrationPage::updateProjectionWindow()
{
    if (stillFrame.empty() || numSelectedPoints != 4) {
        qDebug() << "Cannot update projection window: Still frame is empty or points are not fully selected.";
        return;
    }

    // Create a temporary array to sort points
    std::array<cv::Point2f, 4> sortedPoints = selectedPoints;
    sortPointsClockwise(sortedPoints);
    m_projectionWindow->setStillFrame(stillFrame);
    m_projectionWindow->setTransformCorners(sortedPoints);

    // Set the image in the projection window using updateImage
    m_projectionWindow->setProjectionState(ImageProjectionWindow::projectionState::EDGE_DETECTION);
}

// Update the display using the still frame
void CalibrationPage::updateDisplayWithStillFrame()
{
    if (stillFrame.empty()) {
        return;
    }

    cv::Mat displayFrame = stillFrame.clone();

    // Draw selected points
    for (int i = 0; i < numSelectedPoints; ++i) {
        cv::circle(displayFrame, selectedPoints[i], 5, cv::Scalar(0, 255, 0), -1);
    }

    // Draw ROI
    if (numSelectedPoints == 4) {
        drawROI(displayFrame, selectedPoints);
    }

    // Only draw magnifying glass if dragging is active
    if (dragging && selectedCorner != -1) {
        drawMagnifyingGlass(stillFrame, displayFrame, mouseX, mouseY);
    }

    // Scale down the displayFrame to DISPLAY_WIDTH x DISPLAY_HEIGHT
    cv::Mat scaledFrame;
    cv::resize(displayFrame, scaledFrame, cv::Size(DISPLAY_WIDTH, DISPLAY_HEIGHT), 0, 0, cv::INTER_LINEAR);

    // Convert scaledFrame to QImage
    cv::Mat rgbFrame;
    cv::cvtColor(scaledFrame, rgbFrame, cv::COLOR_BGR2RGB);
    qimg = QImage(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888).copy();

    // Update the widget
    update();
}

// Paint event to render the image on the widget
void CalibrationPage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    if (!qimg.isNull()) {
        // Define the smaller display area (centered)
        int xOffset = (width() - DISPLAY_WIDTH) / 2;
        int yOffset = (height() - DISPLAY_HEIGHT) / 2;
        QRect displayRect(xOffset, yOffset, DISPLAY_WIDTH, DISPLAY_HEIGHT);

        // Draw the image within the displayRect
        painter.drawImage(displayRect, qimg);
    }
}

// Handle mouse press events for point selection and dragging
void CalibrationPage::mousePressEvent(QMouseEvent* event)
{
    // Define the smaller display area (centered)
    int xOffset = (width() - DISPLAY_WIDTH) / 2;
    int yOffset = (height() - DISPLAY_HEIGHT) / 2;

    // Check if the click is within the displayRect
    if (event->x() >= xOffset && event->x() <= (xOffset + DISPLAY_WIDTH) &&
        event->y() >= yOffset && event->y() <= (yOffset + DISPLAY_HEIGHT)) {

        // Map the click position to the image coordinates
        int imgX = event->x() - xOffset;
        int imgY = event->y() - yOffset;

        // Since the image is scaled down to DISPLAY_WIDTH x DISPLAY_HEIGHT,
        // map imgX and imgY back to the original frame coordinates
        float scaleX = static_cast<float>(WIDTH) / DISPLAY_WIDTH;
        float scaleY = static_cast<float>(HEIGHT) / DISPLAY_HEIGHT;

        mouseX = imgX * scaleX;
        mouseY = imgY * scaleY;

        if (event->button() == Qt::LeftButton && numSelectedPoints < 4) {
            if (isValidPoint(cv::Point2f(mouseX, mouseY), 20.0)) {
                selectedPoints[numSelectedPoints] = cv::Point2f(mouseX, mouseY);
                ++numSelectedPoints;
                qDebug() << "Point selected:" << mouseX << "," << mouseY;
                pointsChanged = true; // Points have changed

                // If 4 points are selected, capture the still frame
                if (numSelectedPoints == 4) {
                    stillFrame = frame.clone();
                    stillFrameCaptured = true;
                    timer->stop();
                    updateProjectionWindow();
                    updateDisplayWithStillFrame();
                }
            } else {
                qDebug() << "Point is too close to an existing point.";
            }
        } else if (event->button() == Qt::LeftButton && numSelectedPoints == 4) {
            selectedCorner = findClosestCorner(mouseX, mouseY);
            if (selectedCorner != -1) {
                dragging = true;
            }
        }
    }
}

// Handle mouse move events for dragging points
void CalibrationPage::mouseMoveEvent(QMouseEvent* event)
{
    // Define the smaller display area (centered)
    int xOffset = (width() - DISPLAY_WIDTH) / 2;
    int yOffset = (height() - DISPLAY_HEIGHT) / 2;

    // Check if the mouse is within the displayRect
    if (event->x() >= xOffset && event->x() <= (xOffset + DISPLAY_WIDTH) &&
        event->y() >= yOffset && event->y() <= (yOffset + DISPLAY_HEIGHT)) {

        // Map the mouse position to the image coordinates
        int imgX = event->x() - xOffset;
        int imgY = event->y() - yOffset;

        // Map to original frame coordinates
        float scaleX = static_cast<float>(WIDTH) / DISPLAY_WIDTH;
        float scaleY = static_cast<float>(HEIGHT) / DISPLAY_HEIGHT;

        mouseX = imgX * scaleX;
        mouseY = imgY * scaleY;

        if (dragging && selectedCorner != -1) {
            selectedPoints[selectedCorner] = cv::Point2f(mouseX, mouseY);
            // qDebug() << "Point dragged to:" << mouseX << "," << mouseY;

            pointsChanged = true; // Points have changed

            // Update the projection window with the new point positions
            updateProjectionWindow();

            // Update the display with the still frame
            updateDisplayWithStillFrame();
        }
    }
}

// Handle mouse release events to stop dragging
void CalibrationPage::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (dragging) {
        dragging = false;
        pointsChanged = true;
        selectedCorner = -1;
        qDebug() << "Dragging ended. Magnifying glass hidden.";
        update(); // Refresh the display to hide the magnifying glass

        // Ensure the display is updated to remove the magnifying glass
        updateDisplayWithStillFrame();
    }
}

// Reset all selected points and related states
void CalibrationPage::resetPoints()
{
    numSelectedPoints = 0;
    resetMode = true;
    pointsSelected = false;
    dragging = false;
    selectedCorner = -1;
    mouseX = -1;
    mouseY = -1;
    matrix.release();
    stillFrame.release();
    stillFrameCaptured = false; // Reset the still frame flag
    pointsChanged = false; // Reset the flag
    qimg = QImage(); // Clear the image
    qDebug() << "Points reset. Please select 4 new points.";

    // Clear the image in the projection window using clearImage
    m_projectionWindow->setProjectionState(ImageProjectionWindow::projectionState::SCANNING);

    update();

    // Restart the camera to resume live feed
    startCamera();
}

// Finalize the selection (if needed)
void CalibrationPage::finalizeSelection()
{
    if (numSelectedPoints >= 4) {
        pointsSelected = true;
        updateProjectionWindow();
    }
}

// Slot for the Complete button to navigate to the sensitivity page
void CalibrationPage::onCompleteButtonClicked()
{
    // Navigate to the sensitivity page
    emit navigateToSensitivityPage();
}

// Draw a magnifying glass effect around the cursor position
void CalibrationPage::drawMagnifyingGlass(const cv::Mat& sourceFrame, cv::Mat& drawFrame, int x, int y, int zoomFactor, int radius)
{
    int magnifierOffset = radius + 20;
    bool aboveMouse = (y - magnifierOffset - radius >= 0);
    int magnifierY = aboveMouse ? (y - magnifierOffset) : (y + magnifierOffset);

    int roiXStart = std::max(x - radius / zoomFactor, 0);
    int roiXEnd = std::min(x + radius / zoomFactor, WIDTH);
    int roiYStart = std::max(y - radius / zoomFactor, 0);
    int roiYEnd = std::min(y + radius / zoomFactor, HEIGHT);

    cv::Mat roi = sourceFrame(cv::Rect(cv::Point(roiXStart, roiYStart), cv::Point(roiXEnd, roiYEnd)));

    if (!roi.empty()) {
        cv::Mat magnifiedRoi;
        cv::resize(roi, magnifiedRoi, cv::Size(2 * radius, 2 * radius), 0, 0, cv::INTER_LINEAR);

        cv::Mat mask(magnifiedRoi.size(), CV_8UC1, cv::Scalar(0));
        cv::circle(mask, cv::Point(magnifiedRoi.cols / 2, magnifiedRoi.rows / 2), radius, cv::Scalar(255), -1);

        cv::Mat magnifiedWithMask;
        magnifiedRoi.copyTo(magnifiedWithMask, mask);

        int topLeftX = x - magnifiedRoi.cols / 2;
        int topLeftY = magnifierY - magnifiedRoi.rows / 2;

        if (topLeftX >= 0 && topLeftY >= 0 &&
            topLeftX + magnifiedRoi.cols <= WIDTH &&
            topLeftY + magnifiedRoi.rows <= HEIGHT) {

            cv::Vec3b centerPixelColor = sourceFrame.at<cv::Vec3b>(cv::Point(x, y));
            cv::Scalar circleColor(centerPixelColor[0], centerPixelColor[1], centerPixelColor[2]);

            cv::circle(drawFrame, cv::Point(x, magnifierY), radius + 2, circleColor, 8);

            cv::drawMarker(magnifiedWithMask, cv::Point(magnifiedWithMask.cols / 2, magnifiedWithMask.rows / 2),
                           cv::Scalar(0, 255, 0), cv::MARKER_CROSS, 35, 2);

            magnifiedWithMask.copyTo(drawFrame(cv::Rect(topLeftX, topLeftY, magnifiedWithMask.cols, magnifiedWithMask.rows)), mask);
        }
    }
}

// Draw the Region of Interest (ROI) with grid lines
void CalibrationPage::drawROI(cv::Mat& frame, const std::array<cv::Point2f, 4>& selectedPoints)
{
    // Ensure points are sorted in clockwise order
    std::array<cv::Point2f, 4> sortedPoints = selectedPoints;
    sortPointsClockwise(sortedPoints);

    // Draw the rectangle (ROI)
    for (int i = 0; i < 4; i++) {
        cv::line(frame, sortedPoints[i], sortedPoints[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }

    const float oneThird = 1.0f / 3.0f;
    const float twoThirds = 2.0f / 3.0f;

    // Precompute directional vectors for each pair of points
    cv::Point2f vec1 = sortedPoints[1] - sortedPoints[0];
    cv::Point2f vec2 = sortedPoints[2] - sortedPoints[3];
    cv::Point2f vec3 = sortedPoints[3] - sortedPoints[0];
    cv::Point2f vec4 = sortedPoints[2] - sortedPoints[1];

    // Draw vertical grid lines
    cv::line(frame, sortedPoints[0] + vec1 * oneThird, sortedPoints[3] + vec2 * oneThird, cv::Scalar(0, 255, 0), 1);
    cv::line(frame, sortedPoints[0] + vec1 * twoThirds, sortedPoints[3] + vec2 * twoThirds, cv::Scalar(0, 255, 0), 1);

    // Draw horizontal grid lines
    cv::line(frame, sortedPoints[0] + vec3 * oneThird, sortedPoints[1] + vec4 * oneThird, cv::Scalar(0, 255, 0), 1);
    cv::line(frame, sortedPoints[0] + vec3 * twoThirds, sortedPoints[1] + vec4 * twoThirds, cv::Scalar(0, 255, 0), 1);
}

// Sort points in clockwise order based on their angles from the center
void CalibrationPage::sortPointsClockwise(std::array<cv::Point2f, 4>& points)
{
    cv::Point2f center(0, 0);
    for (const auto& point : points) {
        center += point;
    }
    center *= (1.0f / points.size());

    std::sort(points.begin(), points.end(), [center](const cv::Point2f& a, const cv::Point2f& b) {
        double angleA = std::atan2(a.y - center.y, a.x - center.x);
        double angleB = std::atan2(b.y - center.y, b.x - center.x);
        return angleA < angleB;
    });
}

// Find the closest corner to the given coordinates within a specified radius
int CalibrationPage::findClosestCorner(int x, int y)
{
    const float cornerRadius = 20.0f;
    for (int i = 0; i < numSelectedPoints; ++i) {
        if (cv::norm(selectedPoints[i] - cv::Point2f(x, y)) < cornerRadius) {
            return i;
        }
    }
    return -1;
}

// Validate if the new point is sufficiently distant from existing points
bool CalibrationPage::isValidPoint(const cv::Point2f& newPoint, double minDistance)
{
    for (int i = 0; i < numSelectedPoints; ++i) {
        double dist = cv::norm(selectedPoints[i] - newPoint);
        if (dist < minDistance) {
            return false;
        }
    }
    return true;
}
