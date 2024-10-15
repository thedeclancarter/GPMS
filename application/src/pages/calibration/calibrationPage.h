#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include "imageprojectionwindow.h"
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <vector>

namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPage(ImageProjectionWindow *projectionWindow, QWidget *parent = nullptr);
    ~CalibrationPage();

    void startCamera();
    void stopCamera();

    void resetPoints();
    void finalizeSelection();

private slots:
    void captureFrame();
    void onCompleteButtonClicked(); // Slot for the Complete button

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void navigateToSensitivityPage();

private:
    Ui::CalibrationPage *ui;
    ImageProjectionWindow *m_projectionWindow;

    QTimer* timer;
    cv::VideoCapture cap;
    cv::Mat frame;
    QImage qimg;

    // Point selection variables
    std::vector<cv::Point2f> selectedPoints;
    bool resetMode;
    bool pointsSelected;
    bool dragging;
    int selectedCorner;
    int mouseX, mouseY;

    // Stateful variables
    cv::Mat matrix;
    cv::Mat stillFrame;
    bool stillFrameCaptured;
    bool pointsChanged;

    // Methods
    void processFrame();
    void updateProjectionWindow(); // Method to update the projection window
    void drawMagnifyingGlass(const cv::Mat& sourceFrame, cv::Mat& drawFrame, int x, int y, int zoomFactor = 2, int radius = 50);
    void drawROI(cv::Mat& frame, std::vector<cv::Point2f>& selectedPoints);
    void sortPointsClockwise(std::vector<cv::Point2f>& points);
    int findClosestCorner(int x, int y);
    bool isValidPoint(const cv::Point2f& newPoint, const std::vector<cv::Point2f>& points, double minDistance);
    void updateDisplayWithStillFrame();
};

#endif // CALIBRATIONPAGE_H
