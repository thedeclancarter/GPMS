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
        // Set window flags to keep it behind other windows
        imageProjectionWindow->setWindowFlags(Qt::Window | Qt::WindowStaysOnBottomHint);

        imageProjectionWindow->show();
        imageProjectionWindow->lower(); // Ensure it stays behind other windows
    }
}

void MainWindow::setupConnections()
{
    // logo
    connect(logoButton, &QPushButton::clicked, this, &MainWindow::navigateToCreatePage);

    // from create page
    connect(createPage, &CreatePage::navigateToCalibrationPage, this, &MainWindow::navigateToCalibrationPage);

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

    // from project page
    connect(projectPage, &ProjectPage::navigateToCreatePage, this, &MainWindow::navigateToCreatePage);
    connect(projectPage, &ProjectPage::navigateToPickImagesPage, this, &MainWindow::navigateToPickImagesPage);
}

void MainWindow::navigateToCreatePage()
{
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

void MainWindow::navigateToPickImagesPage()
{
    stackedWidget->setCurrentWidget(pickImagesPage);
}

void MainWindow::navigateToProjectPage(const cv::Mat& selectedImage)
{

    // if (selectedImage.isNull()) {
    //     qDebug() << "Error: selectedImage is null";
    //     return;
    // }

    // qDebug() << "Original QPixmap size:" << selectedImage.size();

    // QImage qImage = selectedImage.toImage();
    // qImage = qImage.convertToFormat(QImage::Format_RGB888);
    // qDebug() << "QImage format:" << qImage.format();

    // cv::Mat cvImage;
    // switch(qImage.format()) {
    // case QImage::Format_RGBA8888:
    //     qDebug() << "Converting from Format_RGBA8888";
    //     cvImage = cv::Mat(qImage.height(), qImage.width(), CV_8UC4, (uchar*)qImage.bits(), qImage.bytesPerLine());
    //     cv::cvtColor(cvImage, cvImage, cv::COLOR_RGBA2BGR);
    //     break;
    // case QImage::Format_RGB888:
    //     qDebug() << "Converting from Format_RGB888";
    //     cvImage = cv::Mat(qImage.height(), qImage.width(), CV_8UC3, (uchar*)qImage.bits(), qImage.bytesPerLine());
    //     cv::cvtColor(cvImage, cvImage, cv::COLOR_RGB2BGR);
    //     break;
    // default:
    //     qDebug() << "Unsupported QImage format:" << qImage.format();
    //     return;
    // }

    // qDebug() << "cvImage size:" << cvImage.size().width << "x" << cvImage.size().height;
    // qDebug() << "cvImage channels:" << cvImage.channels();
    // qDebug() << "cvImage type:" << cvImage.type();

    // // Check if the image has any non-zero pixels
    // cv::Scalar sum = cv::sum(cvImage);
    // qDebug() << "Image sum:" << sum[0] + sum[1] + sum[2] + sum[3];

    // // Save the debug image
    // QString debugImagePath = QDir::currentPath() + "/debug_image.png";
    // cv::imwrite(debugImagePath.toStdString(), cvImage);
    // qDebug() << "Saved debug image to:" << debugImagePath;

    imageProjectionWindow->setStillFrame(selectedImage);
    imageProjectionWindow->setProjectionState(ImageProjectionWindow::projectionState::IMAGE);
    projectPage->setSelectedImage(selectedImage);
    stackedWidget->setCurrentWidget(projectPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}
