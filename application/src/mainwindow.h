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

    QWidget* m_projectionContainer;

    // functions
    void setupUI();
    void setupPages();
    void setupConnections();

    // for projection window
    void showImageProjectionWindow();
    QScreen* findProjectorScreen();


private slots:
    // main page
    void navigateToCreatePage();
    void logoClicked();

    // create page pages
    void navigateToCalibrationPage();
    void navigateToSensitivityPage();
    void navigateToTextVisionPageFromSensitivity(int low, int high);
    void navigateToTextVisionPage();
    void navigateFromProjectPageToPickImagesPage();
    void navigateFromTextVisionToPickImages(QString prompt, bool isRealistic); // in order to pass in vars
    void navigateToProjectPage(const cv::Mat& image);


};
#endif // MAINWINDOW_H
