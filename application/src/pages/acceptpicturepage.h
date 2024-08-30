#ifndef ACCEPTPICTUREPAGE_H
#define ACCEPTPICTUREPAGE_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class AcceptPicturePage;
}

class AcceptPicturePage : public QWidget
{
    Q_OBJECT

    public:
        explicit AcceptPicturePage(QWidget *parent = nullptr);
        ~AcceptPicturePage();

        void setImage(const QImage &image);


    signals:
        void navigateToPicturePage();
        void navigateToSensitivityPage();

    private slots:
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        Ui::AcceptPicturePage *ui;
        QLabel *imageLabel;
};

#endif // ACCEPTPICTUREPAGE_H
