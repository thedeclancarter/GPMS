#ifndef ACCEPTPICTUREPAGE_H
#define ACCEPTPICTUREPAGE_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

namespace Ui {
class AcceptPicturePage;
}

class AcceptPicturePage : public QWidget
{
    Q_OBJECT

    public:
        explicit AcceptPicturePage(QWidget *parent = nullptr);
        ~AcceptPicturePage();
        // from take picture
        void setImage(const QImage &image);


    signals:
        void navigateToPicturePage();
        void navigateToSensitivityPage(const QImage &image);
        // to sensitivity page
        // void acceptedImage(const QImage &image);

    private slots:
        // in accept picture
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        void initializeUI();
        QLabel* createTitleLabel();
        QFrame* createImageFrame();
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);

        Ui::AcceptPicturePage *ui;
        // QLabel *imageLabel;
        QImage currentImage; // store the current image
};

#endif // ACCEPTPICTUREPAGE_H
