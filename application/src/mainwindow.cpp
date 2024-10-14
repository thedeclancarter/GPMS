#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QGraphicsDropShadowEffect>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "Qt version:" << QT_VERSION_STR;
    ui->setupUi(this);
    setupUI();
    qDebug("Finished setting up UI");
    setupPages();
    qDebug("Finished setting up all functions");
    setupConnections();
    qDebug("Finished making all connections");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    this->setStyleSheet("background-color: #1C1C1E;");

    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout();

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
    QPixmap logo(":/icons/GPMS_logo2.webp");
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
    createPage = new CreatePage(this);

    imageProjectionWindow = new ImageProjectionWindow();
    imageProjectionWindow->setAttribute(Qt::WA_DeleteOnClose);

    // variable in the .h
    calibrationPage = new CalibrationPage(imageProjectionWindow, this);

    sensitivityPage = new SensitivityPage(this);
    textVisionPage = new TextVisionPage(this);
    pickImagesPage = new PickImagesPage(this);
    projectPage = new ProjectPage(this);

    stackedWidget->addWidget(createPage);

    stackedWidget->addWidget(calibrationPage);
    stackedWidget->addWidget(sensitivityPage);
    stackedWidget->addWidget(textVisionPage);
    stackedWidget->addWidget(pickImagesPage);
    stackedWidget->addWidget(projectPage);

    // Start with the create page
    stackedWidget->setCurrentWidget(createPage);
}

void MainWindow::setupConnections()
{
    // logo
    connect(logoButton, &QPushButton::clicked, this, &MainWindow::navigateToCreatePage);

    // from create page
    connect(createPage, &CreatePage::navigateToCalibrationPage, this, &MainWindow::showProjectionWindow);

    // from calibration page
    connect(calibrationPage, &CalibrationPage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);

    // from sensitivity page
    connect(sensitivityPage, &SensitivityPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(sensitivityPage, &SensitivityPage::navigateToCalibrationPage, this, &MainWindow::navigateToCalibrationPage);

    // from text vision page
    connect(textVisionPage, &TextVisionPage::navigateToPickImagesPage, this, &MainWindow::navigateToPickImagesPage);
        // take picture here when clicked

    // from pick images page
    connect(pickImagesPage, &PickImagesPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(pickImagesPage, &PickImagesPage::navigateToProjectPage, this, &MainWindow::navigateToProjectPage);
    connect(pickImagesPage, &PickImagesPage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);    
}

void MainWindow::showProjectionWindow()
{
    if (imageProjectionWindow) {
        imageProjectionWindow->show();
        imageProjectionWindow->lower();
        stackedWidget->setCurrentWidget(calibrationPage);
    }
    else{
        qDebug("In showProjectionWindow imageprojectionwindow is null");
    }
}

void MainWindow::navigateToCreatePage()
{
    stackedWidget->setCurrentWidget(createPage);
}

void MainWindow::navigateToCalibrationPage()
{
    stackedWidget->setCurrentWidget(calibrationPage);
}

void MainWindow::navigateToSensitivityPage()
{
    if (imageProjectionWindow) {
        sensitivityPage->setProjectionWindow(imageProjectionWindow);
    } else {
        qDebug("Error: imageProjectionWindow is null in navigateToSensitivityPage");
    }
    stackedWidget->setCurrentWidget(sensitivityPage);
}

void MainWindow::navigateToTextVisionPage()
{
    stackedWidget->setCurrentWidget(textVisionPage);
}

void MainWindow::navigateToPickImagesPage()
{
    stackedWidget->setCurrentWidget(pickImagesPage);
}

void MainWindow::navigateToProjectPage()
{
    stackedWidget->setCurrentWidget(projectPage);
}

// pictures

void MainWindow::setImageForAcceptPage(const QImage &image)
{
    // Assuming you have an AcceptPicturePage class
    // acceptPicturePage->setImage(image);
}
