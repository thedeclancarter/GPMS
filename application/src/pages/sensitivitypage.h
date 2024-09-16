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
#include <QCameraImageCapture>

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
        ImageProjectionWindow *m_projectionWindow;

    signals:
        void navigateToTextVisionPage();

    private slots:
        void onAcceptButtonClicked();
        void captureAndProcessFrame();

    private:
        Ui::SensitivityPage *ui;

        QLabel *m_imageLabel;
        QSlider *lowerSlider;
        QSlider *upperSlider;
        QTimer *timer;
        QCamera *m_camera;
        QCameraViewfinder *m_viewfinder;
        QCameraImageCapture *m_imageCapture;
        QVideoFrame *m_lastFrame;

        void init();
        void initializeUI();
        bool checkCameraAvailability();

        void initializeUIWithoutCamera();
        void restartApplication();

        QLabel* createTitleLabel();
        QFrame* createImageFrame();
        QSlider* createSlider(QSlider* slider);
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);

        void setupCamera();
        void captureImage();
        // void processFrame(const QVideoFrame &frame);
        void processFrame(int id, const QVideoFrame &frame);
        void updateDisplays(const QImage &image);
        // void processFrame();
        // void applyCannyEdgeDetection(int lowerThreshold, int upperThreshold);
};

#endif // SENSITIVITYPAGE_H
