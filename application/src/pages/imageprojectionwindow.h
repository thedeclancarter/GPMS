#ifndef IMAGEPROJECTIONWINDOW_H
#define IMAGEPROJECTIONWINDOW_H

#include <QWindow>
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

    explicit ImageProjectionWindow(QWidget* parent = nullptr);

    // Setters
    void setStillFrame(const cv::Mat &image);
    void setFinalFrame(const cv::Mat &mat);
    void setSensitivity(int lo, int hi);
    void setTransformCorners(const std::array<cv::Point2f, 4>& transformCorners);
    void setProjectionState(projectionState state);

    // Getters
    bool getIsCalibrated(void) const;
    QImage getCurrentImage() const;

    // Functions
    void showOnProjector();

private:
    static constexpr int WIDTH = 1280, HEIGHT = 720;

    // image for proj
    cv::Mat m_stillFrame;
    cv::Mat m_finalFrame;

    QLabel *m_imageLabel; // holding the image on screen

    // Cached Values
    bool m_updatePerspectiveMatrix = true;
    bool m_updateEdgeDetectionFrame = true;
    cv::Mat m_perspectiveMatrix;
    cv::Mat m_edgeDetectionFrame;

    int m_loSensitivity, m_hiSensitivity;
    std::array<cv::Point2f, 4> m_transformCorners;

    QTimer *m_rainbowTimer;
    int m_frameCount;

    bool m_isCalibrated = false;
    projectionState m_state;

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


    // ui functions
    void setupUI();
    void debugPositionInfo();

    // for the projector screen
    QScreen* findProjectorScreen();
    void moveToScreen(QScreen* screen);
    void setupProjectorMode();
    const QSize FIXED_SIZE{1280, 720};
    bool m_isOnProjector = false;


private slots:
    void updateRainbowEdges();
};

#endif // IMAGEPROJECTIONWINDOW_H
