#ifndef SENSITIVITYPAGE_H
#define SENSITIVITYPAGE_H

#include <QWidget>
#include <QLabel>

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
        QLabel *imageLabel;
        QImage currentImage;
        void updateImageDisplay();
};

#endif // SENSITIVITYPAGE_H
