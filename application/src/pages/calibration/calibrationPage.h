#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include "windows/imageprojectionwindow.h"
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <array>
#include <QHBoxLayout>
#include <QPushButton>

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
    QPixmap getImage();
    QImage getQImage();
    QImage getCleanQImage();

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
    QLabel* m_imageLabel; // for image on monitor

    // Point selection variables
    std::array<cv::Point2f, 4> selectedPoints;
    int numSelectedPoints = 0;
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
    void drawMagnifyingGlass(const cv::Mat& sourceFrame, cv::Mat& drawFrame, int x, int y, int zoomFactor = 2, int radius = 80);
    void drawROI(cv::Mat& frame, const std::array<cv::Point2f, 4>& selectedPoints);
    void sortPointsClockwise(std::array<cv::Point2f, 4>& points);
    int findClosestCorner(int x, int y);
    bool isValidPoint(const cv::Point2f& newPoint, double minDistance);
    void updateDisplayWithStillFrame();

    // UI Functions
    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createImageFrame();
    QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
    QHBoxLayout* createButtonLayout();
    void updateImage(const QImage& image); // to update image on monitor
};


#endif // CALIBRATIONPAGE_H
