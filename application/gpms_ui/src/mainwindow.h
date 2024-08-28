#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "pages/createpage.h"
#include "pages/picturepage.h"
#include "pages/acceptpicturepage.h"
#include "pages/sensitivitypage.h"
#include "pages/textvisionpage.h"
#include "pages/pickimagespage.h"
#include "pages/projectpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        void setupUI();
        void setupPages();
        void setupConnections();

        Ui::MainWindow *ui;
        QStackedWidget *stackedWidget;

        CreatePage *createPage;
        PicturePage *picturePage;
        AcceptPicturePage *acceptPicturePage;
        SensitivityPage *sensitivityPage;
        TextVisionPage *textVisionPage;
        PickImagesPage *pickImagesPage;
        ProjectPage *projectPage;

    private slots:
        void navigateToPicturePage();
        void navigateToAcceptPicturePage();
        void navigateToSensitivityPage();
        void navigateToTextVisionPage();
        void navigateToPickImagesPage();
        void navigateToProjectPage();
};
#endif // MAINWINDOW_H
