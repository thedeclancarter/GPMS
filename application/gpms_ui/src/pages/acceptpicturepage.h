#ifndef ACCEPTPICTUREPAGE_H
#define ACCEPTPICTUREPAGE_H

#include <QWidget>

namespace Ui {
class AcceptPicturePage;
}

class AcceptPicturePage : public QWidget
{
    Q_OBJECT

    public:
        explicit AcceptPicturePage(QWidget *parent = nullptr);
        ~AcceptPicturePage();

    signals:
        void navigateToPicturePage();
        void navigateToSensitivityPage();

    private slots:
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        Ui::AcceptPicturePage *ui;
};

#endif // ACCEPTPICTUREPAGE_H
