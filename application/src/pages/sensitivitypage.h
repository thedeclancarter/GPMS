#ifndef SENSITIVITYPAGE_H
#define SENSITIVITYPAGE_H
#include "imageprojectionwindow.h"
#include <QWidget>
#include <QLabel>
#include <QtWidgets/qslider.h>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>
#include <opencv2/opencv.hpp>
#include <QCamera>
#include <QCameraViewfinder>

namespace Ui {
class SensitivityPage;
}

class SensitivityPage : public QWidget
{
    Q_OBJECT

    public:
        explicit SensitivityPage(QWidget *parent = nullptr);
        ~SensitivityPage();
        void setProjectionWindow(ImageProjectionWindow *projectionWindow);

        // void setAcceptedImage(const QImage &image);

    signals:
        void navigateToTextVisionPage();

    private slots:
        void onAcceptButtonClicked();

    private:
        Ui::SensitivityPage *ui;
        ImageProjectionWindow *m_projectionWindow;
        QCameraViewfinder *m_viewfinder;
        QCamera *m_camera;

        QImage currentImage;
        QSlider *lowerSlider;
        QSlider *upperSlider;

        cv::VideoCapture capture;
        QTimer *timer;
        QImage currentFrame;

        void init();
        void initializeUI();
        bool checkCameraAvailability();
        void setupCamera();
        void processFrame();
        void applyCannyEdgeDetection(int lowerThreshold, int upperThreshold);
        void captureAndProcessFrame();

        QLabel* createTitleLabel();
        QFrame* createImageFrame();
        QSlider* createSlider(QSlider* slider);
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
};

#endif // SENSITIVITYPAGE_H
