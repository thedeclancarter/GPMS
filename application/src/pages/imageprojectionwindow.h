#ifndef IMAGEPROJECTIONWINDOW_H
#define IMAGEPROJECTIONWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QTimer>
#include <opencv2/opencv.hpp>

class ImageProjectionWindow : public QWidget
{
    Q_OBJECT

public:
    typedef enum class projectionState {
        LOGO,
        SCANNING,
        EDGE_DETECTION,
        RAINBOW_EDGE,
        IMAGE
    } projectionState;

    explicit ImageProjectionWindow(QWidget *parent = nullptr);

    // Setters
    void setStillFrame(const cv::Mat &image);
    void setSensitivity(int lo, int hi);
    void setTransformCorners(const std::array<cv::Point2f, 4>& transformCorners);
    void setProjectionState(projectionState state);

    // Getters
    bool getIsCalibrated(void) const;
    QImage getCurrentImage() const;

private:
    static constexpr int WIDTH = 1280, HEIGHT = 720;

    cv::Mat m_stillFrame;
    int m_loSensitivity, m_hiSensitivity;
    std::array<cv::Point2f, 4> m_transformCorners;
    projectionState m_state;
    bool m_isCalibrated = false;

    QLabel *m_imageLabel;

    QTimer *m_rainbowTimer;
    int m_frameCount;

    void setupUI();

    // State Transitions
    void activateLogo();
    void activateScanning();
    void activateEdgeDetection();
    void activateRainbowEdge();
    void activateImage();

    // Helper functions
    void updateImage(const cv::Mat &mat);
    void updateImage(const QImage &image);
    cv::Mat applyPerspectiveTransform(const cv::Mat& mat);

private slots:
    void updateRainbowEdges();
};

#endif // IMAGEPROJECTIONWINDOW_H
