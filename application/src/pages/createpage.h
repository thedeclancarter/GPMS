#ifndef CREATEPAGE_H
#define CREATEPAGE_H
#include "imageprojectionwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class CreatePage;
}

class CreatePage : public QWidget
{
    Q_OBJECT

    public:
        explicit CreatePage(ImageProjectionWindow *projectionWindow, QWidget *parent = nullptr);
        ~CreatePage();
        void startCamera();
        void stopCamera();

    signals:
        void navigateToCalibrationPage();
        void goBack();

    private slots:
        void onCreateButtonClicked();
        void captureFrame();

    private:
        Ui::CreatePage *ui;
        QPushButton *createButton;
        QPushButton *backButton;
        ImageProjectionWindow *m_projectionWindow;

        QLabel *previewLabel;

        QTimer* timer;
        cv::VideoCapture cap;
        cv::Mat frame;
        QImage qimg;
        QLabel* m_imageLabel; // for image on monitor

        void setupUI();
        void setupHeader(QVBoxLayout *layout);

        void setupStepsSection(QHBoxLayout *layout);
        void setupImagePreviewSection(QHBoxLayout *layout);
        void setupCreateButton(QVBoxLayout *layout);

        void setupConnections();

        void setupLeftColumn(QGridLayout *layout, int row, int column);
        void setupStepsSection(QVBoxLayout *layout);
        void setupRightColumn(QGridLayout *layout, int row, int column);
};

#endif // CREATEPAGE_H
