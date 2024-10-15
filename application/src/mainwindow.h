#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

#include "pages/createpage.h"
#include "pages/takepicture.h"

#include "pages/calibration/calibrationPage.h"
#include "pages/sensitivitypage.h"
#include "pages/textvisionpage.h"
#include "pages/pickimagespage.h"
#include "pages/projectpage.h"

#include "pages/imageprojectionwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // image projection page
    ImageProjectionWindow *imageProjectionWindow;


private:

    Ui::MainWindow *ui;
    QPushButton *logoButton;
    QStackedWidget *stackedWidget;

    // pages
    CreatePage *createPage;
    TakePicture *takePicture;

    CalibrationPage *calibrationPage;
    SensitivityPage *sensitivityPage;
    TextVisionPage *textVisionPage;
    PickImagesPage *pickImagesPage;
    ProjectPage *projectPage;

    // functions
    void setupUI();
    void setupPages();
    void setupConnections();

    void showProjectionWindow();

    QWidget* createSidebar();
    QPushButton* createSidebarButton(const QIcon& icon);

private slots:
    // main page
    void navigateToCreatePage();

    // create page pages
    void navigateToCalibrationPage();
    void navigateToSensitivityPage();
    void navigateToTextVisionPage();
    void navigateToPickImagesPage();
    void navigateToProjectPage();

    // for the picture
    void setImageForAcceptPage(const QImage &image);

};
#endif // MAINWINDOW_H
