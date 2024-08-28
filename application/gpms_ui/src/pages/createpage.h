#ifndef CREATEPAGE_H
#define CREATEPAGE_H

#include <QWidget>

namespace Ui {
class CreatePage;
}

class CreatePage : public QWidget
{
    Q_OBJECT

    public:
        explicit CreatePage(QWidget *parent = nullptr);
        ~CreatePage();

    signals:
        void navigateToPicturePage();

    private slots:
        void onCreateButtonClicked();

    private:
        Ui::CreatePage *ui;
};

#endif // CREATEPAGE_H
