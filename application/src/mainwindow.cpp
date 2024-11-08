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
    imageProjectionWindow = new ImageProjectionWindow();
    imageProjectionWindow->setAttribute(Qt::WA_DeleteOnClose);
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
        qDebug() << "Before showing window:";
        qDebug() << "  Window geometry:" << imageProjectionWindow->geometry();
        qDebug() << "  Window visibility:" << imageProjectionWindow->isVisible();
        qDebug() << "  Window flags:" << imageProjectionWindow->windowFlags();


        // Set window flags to keep it behind other windows
        imageProjectionWindow->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

        imageProjectionWindow->show();

        // Move to projector if available
        moveToProjector();

        // imageProjectionWindow->lower(); // Ensure it stays behind other windows

        qDebug() << "After showing window:";
        qDebug() << "  Window geometry:" << imageProjectionWindow->geometry();
        qDebug() << "  Window visibility:" << imageProjectionWindow->isVisible();
        qDebug() << "  Window flags:" << imageProjectionWindow->windowFlags();
    }
}

void MainWindow::moveToProjector()
{
    QScreen* projectorScreen = findProjectorScreen();
    if (projectorScreen) {
        QRect screenGeometry = projectorScreen->geometry();
        qDebug() << "Moving window to projector:";
        qDebug() << "  Target screen:" << projectorScreen->name();
        qDebug() << "  Target geometry:" << screenGeometry;
        qDebug() << "  Current window geometry:" << imageProjectionWindow->geometry();

        imageProjectionWindow->move(screenGeometry.x(), screenGeometry.y());

        qDebug() << "  New window geometry:" << imageProjectionWindow->geometry();

        imageProjectionWindow->raise();
        imageProjectionWindow->activateWindow();
    }
    else {
        qDebug() << "No projector screen found in moveToProjector()";
    }
}

QScreen* MainWindow::findProjectorScreen()
{
    const QList<QScreen*>& screens = QGuiApplication::screens();
    qDebug() << "Number of screens found:" << screens.size();

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    qDebug() << "Primary screen:" << primaryScreen->name()
             << "Geometry:" << primaryScreen->geometry()
             << "Physical size:" << primaryScreen->physicalSize();

    for (QScreen* screen : qAsConst(screens)) {
        qDebug() << "Screen:" << screen->name()
        << "\n  Geometry:" << screen->geometry()
        << "\n  Physical size:" << screen->physicalSize()
        << "\n  Manufacturer:" << screen->manufacturer()
        << "\n  Model:" << screen->model()
        << "\n  Is primary?" << (screen == primaryScreen);
    }

    if (screens.size() <= 1) {
        qDebug() << "No secondary screen found";
        return nullptr;
    }

    // Find first non-primary screen
    for (QScreen* screen : qAsConst(screens)) {
        if (screen != primaryScreen) {
            qDebug() << "Selected projector screen:" << screen->name();
            return screen;
        }
    }

    return nullptr;
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
    connect(sensitivityPage, &SensitivityPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
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
    }
    stackedWidget->setCurrentWidget(calibrationPage);
}

void MainWindow::navigateToSensitivityPage()
{
    stackedWidget->setCurrentWidget(sensitivityPage);
    sensitivityPage->updateSensitivity();
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

// to only refresh when nav from textVision
void MainWindow::navigateFromTextVisionToPickImages()
{
    stackedWidget->setCurrentWidget(pickImagesPage);
    // pickImagesPage->refreshImages();

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
