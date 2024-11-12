#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "Qt version:" << QT_VERSION_STR;
    ui->setupUi(this);
    setupUI();
    setupPages();
    setupConnections();
}

void MainWindow::setupUI()
{
    this->setStyleSheet("background-color: #1C1C1E;");

    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);

    // Create a QStackedWidget
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    // Create a central widget for the main window
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);

    // Set the central widget
    setCentralWidget(centralWidget);

    // Create and set up the logo
    logoButton = new QPushButton(this);
    QPixmap logo(":/icons/GPMS_logo2.png");
    logoButton->setIcon(QIcon(logo));
    logoButton->setIconSize(QSize(60, 60));  // Set the size of the icon
    logoButton->setFixedSize(60, 60);  // Set the size of the button
    logoButton->setCursor(Qt::PointingHandCursor);
    logoButton->setToolTip("Go to Create Page");

    // Style the button to look like a label
    logoButton->setStyleSheet(
        "QPushButton { border: none; background-color: transparent; }"
        // "QPushButton:hover { background-color: rgba(255, 255, 255, 30); }"  // Optional: add hover effect
        );

    logoButton->move(35, 35);  // X, Y
    logoButton->raise(); // Ensure the logo is always on top
}

void MainWindow::setupPages()
{
    // window will be passed to all windows that use it
    imageProjectionWindow = new ImageProjectionWindow(this);
    imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::LOGO);
    showImageProjectionWindow();

    // will show GPMS logo
    createPage = new CreatePage(imageProjectionWindow, this);
    // will show white
    calibrationPage = new CalibrationPage(imageProjectionWindow, this);
    // will show edge detection
    sensitivityPage = new SensitivityPage(imageProjectionWindow, this);
    // what will these show
    textVisionPage = new TextVisionPage(this);
    pickImagesPage = new PickImagesPage(imageProjectionWindow, this);
    // will show projected image
    projectPage = new ProjectPage(imageProjectionWindow, this);

    stackedWidget->addWidget(createPage);

    stackedWidget->addWidget(calibrationPage);
    stackedWidget->addWidget(sensitivityPage);
    stackedWidget->addWidget(textVisionPage);
    stackedWidget->addWidget(pickImagesPage);
    stackedWidget->addWidget(projectPage);

    // Start with the create page
    stackedWidget->setCurrentWidget(createPage);
}

void MainWindow::showImageProjectionWindow()
{
    if (imageProjectionWindow) {
        imageProjectionWindow->showOnProjector();
    }
    else{
        qDebug("Not shown on proj");
    }

    // Debug info
    qDebug() << "Window geometry:" << imageProjectionWindow->geometry();
    qDebug() << "Window visible:" << imageProjectionWindow->isVisible();
}


void MainWindow::setupConnections()
{
    // logo
    connect(logoButton, &QPushButton::clicked, this, &MainWindow::logoClicked);

    // from create page
    connect(createPage, &CreatePage::navigateToCalibrationPage, this, &MainWindow::navigateToCalibrationPage);

    // from calibration page
    connect(calibrationPage, &CalibrationPage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);

    // from sensitivity page
    connect(sensitivityPage, &SensitivityPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPageFromSensitivity);
    connect(sensitivityPage, &SensitivityPage::navigateToCalibrationPage, this, &MainWindow::navigateToCalibrationPage);

    // from text vision page
    connect(textVisionPage, &TextVisionPage::navigateToPickImagesPage, this, &MainWindow::navigateFromTextVisionToPickImages);
        // take picture here when clicked

    // from pick images page
    connect(pickImagesPage, &PickImagesPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(pickImagesPage, &PickImagesPage::navigateToProjectPage, this, &MainWindow::navigateToProjectPage);
    connect(pickImagesPage, &PickImagesPage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);

    // from project page
    connect(projectPage, &ProjectPage::navigateToCreatePage, this, &MainWindow::navigateToCreatePage);
    connect(projectPage, &ProjectPage::navigateToPickImagesPage, this, &MainWindow::navigateFromProjectPageToPickImagesPage);
    connect(projectPage, &ProjectPage::requestImageRefresh, pickImagesPage, &PickImagesPage::refreshImages);
}

void MainWindow::logoClicked(){
    navigateToCreatePage();
    pickImagesPage->refreshImages();
}

void MainWindow::navigateToCreatePage()
{
    // reset everything
    calibrationPage->resetPoints(); // points for calibration
    sensitivityPage->resetSensitivitySliders(); // reset sensitivity bars
    textVisionPage->clearInput();// clear textbox
    pickImagesPage->clearSelections();// selected photos

    // proj window should show video, currently will be still image
    imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::LOGO);
    stackedWidget->setCurrentWidget(createPage);

    // proj window should show white
    if (imageProjectionWindow->getIsCalibrated())
    {
        createPage->startCamera();
    }
    else
    {
        calibrationPage->stopCamera();
        createPage->startCamera();
    }
}

void MainWindow::navigateToCalibrationPage()
{
    // proj window should show white
    if (imageProjectionWindow->getIsCalibrated())
    {
        imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::EDGE_DETECTION);
    }
    else
    {
        imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::SCANNING);
        calibrationPage->startCamera();
    }
    stackedWidget->setCurrentWidget(calibrationPage);
}

void MainWindow::navigateToSensitivityPage()
{
    stackedWidget->setCurrentWidget(sensitivityPage);
    sensitivityPage->updateSensitivity();
    QImage image = calibrationPage->getCleanQImage();
    pickImagesPage->setAPIImage(image);
}

void MainWindow::navigateToTextVisionPageFromSensitivity(int low, int high)
{
    imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::RAINBOW_EDGE);

    stackedWidget->setCurrentWidget(textVisionPage);
    // set threshold here
    pickImagesPage->setLowThreshold(low);
    pickImagesPage->setHighThreshold(high);

}

void MainWindow::navigateToTextVisionPage()
{
    imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::RAINBOW_EDGE);
    stackedWidget->setCurrentWidget(textVisionPage);
}

void MainWindow::navigateFromProjectPageToPickImagesPage()
{
    stackedWidget->setCurrentWidget(pickImagesPage);
}

// to pass in from text vision page
void MainWindow::navigateFromTextVisionToPickImages(QString prompt, bool isRealistic)
{
    stackedWidget->setCurrentWidget(pickImagesPage);
    pickImagesPage->setPrompt(prompt);
    pickImagesPage->setIsRealistic(isRealistic);
    pickImagesPage->fetchRandomImages(2);
}

void MainWindow::navigateToProjectPage(const cv::Mat& selectedImage)
{
    projectPage->setSelectedImage(selectedImage);
    stackedWidget->setCurrentWidget(projectPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}
