#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

#include "pages/create/createpage.h"
#include "pages/calibration/calibrationPage.h"
#include "pages/sensitivity/sensitivitypage.h"
#include "pages/textVision/textvisionpage.h"
#include "pages/pickImages/pickimagespage.h"
#include "pages/project/projectpage.h"

#include "windows/imageprojectionwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // for the pages
    enum class Page {
        CREATE,
        CALIBRATION,
        SENSITIVITY,
        TEXT_VISION,
        PICK_IMAGES,
        PROJECT
    };

    // image projection page
    ImageProjectionWindow *imageProjectionWindow;


private:
    QPushButton *logoButton;
    QStackedWidget *stackedWidget;

    // pages
    CreatePage *createPage;
    CalibrationPage *calibrationPage;
    SensitivityPage *sensitivityPage;
    TextVisionPage *textVisionPage;
    PickImagesPage *pickImagesPage;
    ProjectPage *projectPage;

    Page currentPage = Page::CREATE;

    // functions
    void setupUI();
    void setupPages();
    void setupConnections();
    void navigateToPage(Page page);

    // for projection window
    void showImageProjectionWindow();


private slots:
    void navigateToCreatePage();
    void navigateToCalibrationPage();
    void navigateToSensitivityPage();
    void navigateToTextVisionPage(int low, int high);
    void navigateToPickImagesPage(QString prompt, bool isRealistic); // in order to pass in vars
    void navigateToProjectPage(const cv::Mat& image = cv::Mat());


};
#endif // MAINWINDOW_H
