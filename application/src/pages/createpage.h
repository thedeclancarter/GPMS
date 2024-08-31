#ifndef CREATEPAGE_H
#define CREATEPAGE_H

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
        explicit CreatePage(QWidget *parent = nullptr);
        ~CreatePage();

    signals:
        void navigateToPicturePage();

    private slots:
        void onCreateButtonClicked();

    private:
        Ui::CreatePage *ui;
        QPushButton *createButton;

        void setupUI();
        void setupHeader(QVBoxLayout *layout);
        void setupStepsSection(QHBoxLayout *layout);
        void setupImagePreviewSection(QHBoxLayout *layout);
        void setupCreateButton(QVBoxLayout *layout);
        void setupConnections();
};

#endif // CREATEPAGE_H
