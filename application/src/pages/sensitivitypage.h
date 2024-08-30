#ifndef SENSITIVITYPAGE_H
#define SENSITIVITYPAGE_H

#include <QWidget>

namespace Ui {
class SensitivityPage;
}

class SensitivityPage : public QWidget
{
    Q_OBJECT

    public:
        explicit SensitivityPage(QWidget *parent = nullptr);
        ~SensitivityPage();

    signals:
        void navigateToPicturePage();
        void navigateToTextVisionPage();

    private slots:
        void onAcceptButtonClicked();
        void onRejectButtonClicked();

    private:
        Ui::SensitivityPage *ui;
};

#endif // SENSITIVITYPAGE_H
