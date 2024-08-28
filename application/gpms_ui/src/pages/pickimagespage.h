#ifndef PICKIMAGESPAGE_H
#define PICKIMAGESPAGE_H

#include <QWidget>

namespace Ui {
class PickImagesPage;
}

class PickImagesPage : public QWidget
{
    Q_OBJECT

    public:
        explicit PickImagesPage(QWidget *parent = nullptr);
        ~PickImagesPage();

    signals:
        void navigateToTextVisionPage();
        void navigateToProjectPage();

    private slots:
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        Ui::PickImagesPage *ui;
};

#endif // PICKIMAGESPAGE_H
