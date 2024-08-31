#ifndef SENSITIVITYPAGE_H
#define SENSITIVITYPAGE_H

#include <QWidget>
#include <QLabel>
#include <QtWidgets/qslider.h>
#include <QSlider>
#include <QHBoxLayout>
#include <QPushButton>

namespace Ui {
class SensitivityPage;
}

class SensitivityPage : public QWidget
{
    Q_OBJECT

    public:
        explicit SensitivityPage(QWidget *parent = nullptr);
        ~SensitivityPage();

        void setAcceptedImage(const QImage &image);

    signals:
        void navigateToPicturePage();
        void navigateToTextVisionPage();

    private slots:
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        Ui::SensitivityPage *ui;
        QImage currentImage;
        QSlider *lowerSlider;
        QSlider *upperSlider;

        void initializeUI();
        QLabel* createTitleLabel();
        QFrame* createImageFrame();
        QSlider* createSlider(QSlider* slider);
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);

        void applyCannyEdgeDetection(int lowerThreshold, int upperThreshold);
};

#endif // SENSITIVITYPAGE_H
