#ifndef TEXTVISIONPAGE_H
#define TEXTVISIONPAGE_H

#include <QWidget>

namespace Ui {
class TextVisionPage;
}

class TextVisionPage : public QWidget
{
    Q_OBJECT

    public:
        explicit TextVisionPage(QWidget *parent = nullptr);
        ~TextVisionPage();

    signals:
        void navigateToPickImagesPage();

    private slots:
        void onSubmitButtonClicked();

    private:
        Ui::TextVisionPage *ui;
};

#endif // TEXTVISIONPAGE_H
