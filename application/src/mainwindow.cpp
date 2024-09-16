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

    // Create and add sidebar
    QWidget *sidebar = createSidebar();
    mainLayout->addWidget(sidebar);


    // Create a QStackedWidget
    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    // Create a central widget for the main window
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);

    // Set the central widget
    setCentralWidget(centralWidget);
}


QPushButton* MainWindow::createSidebarButton(const QIcon& icon)
{
    QPushButton *button = new QPushButton();
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setFixedSize(60, 60);
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #1E1E1E;"
        "   border-radius: 15px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4D4D4D;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5D5D5D;"
        "}"
        );

    return button;
}

QWidget* MainWindow::createSidebar()
{
    QWidget *sidebar = new QWidget(this);
    sidebar->setObjectName("sideMenuBar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);

    // Create sidebar buttons with icons
    userButton = createSidebarButton(QIcon(":/icons/user-solid.svg"));
    favoriteButton = createSidebarButton(QIcon(":/icons/heart-solid.svg"));
    createButton = createSidebarButton(QIcon(":/icons/create.svg"));
    settingsButton = createSidebarButton(QIcon(":/icons/gear-solid.svg"));

    // Add buttons to the sidebar layout with centering
    sidebarLayout->addWidget(userButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(favoriteButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(createButton, 0, Qt::AlignCenter);
    sidebarLayout->addStretch(1); // This pushes the settings button to the bottom
    sidebarLayout->addWidget(settingsButton, 0, Qt::AlignCenter);

    // Set layout properties for even spacing
    sidebarLayout->setAlignment(Qt::AlignCenter);
    sidebarLayout->setSpacing(20); // Adjust this value for desired spacing between buttons
    sidebarLayout->setContentsMargins(10, 20, 10, 20); // Add some padding inside the sidebar

    sidebar->setFixedWidth(90); // Set a fixed width for the sidebar
    // Style the sidebar
    sidebar->setStyleSheet(
        "QWidget {"
        "   background-color: #2C2C2E;"
        "   border-radius: 45px;"
        "}"
        );

    // Create and apply the shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(sidebar);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 80));  // Semi-transparent black
    shadowEffect->setOffset(5, 5);  // Offset to bottom-right
    sidebar->setGraphicsEffect(shadowEffect);

    return sidebar;
}

void MainWindow::setupPages()
{
    // create instance of page
    userPage = new UserPage(this);
    favoritesPage = new FavoritesPage(this);
    settingsPage = new SettingsPage(this);

    createPage = new CreatePage(this);

    imageProjectionWindow = new ImageProjectionWindow();
    imageProjectionWindow->setAttribute(Qt::WA_DeleteOnClose);

    // variable in the .h
    calibrationPage = new CalibrationPage(imageProjectionWindow, this);

    sensitivityPage = new SensitivityPage(this);
    textVisionPage = new TextVisionPage(this);
    pickImagesPage = new PickImagesPage(this);
    projectPage = new ProjectPage(this);

    // add each page
    stackedWidget->addWidget(userPage);
    stackedWidget->addWidget(favoritesPage);
    stackedWidget->addWidget(settingsPage);

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
    // Connect buttons to navigation slots
    connect(userButton, &QPushButton::clicked, this, &MainWindow::navigateToUserPage);
    connect(favoriteButton, &QPushButton::clicked, this, &MainWindow::navigateToFavoritesPage);
    connect(createButton, &QPushButton::clicked, this, &MainWindow::navigateToCreatePage);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::navigateToSettingsPage);


    // from create page
    // connect(createPage, &CreatePage::navigateToCalibrationPage, this, &MainWindow::navigateToCalibrationPage);
        // connect to make projection page
    connect(createPage, &CreatePage::navigateToCalibrationPage, this, &MainWindow::showProjectionWindow);

    // from calibration page
    connect(calibrationPage, &CalibrationPage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);

    // from sensitivity page
    connect(sensitivityPage, &SensitivityPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    // change to navigateToCalibrationPage

    // from text vision page
    connect(textVisionPage, &TextVisionPage::navigateToPickImagesPage, this, &MainWindow::navigateToPickImagesPage);
        // take picture here when clicked

    // from take picture page
        // passing the image from take picture to accept
    // connect(takePicture, &TakePicture::imageCaptured, this, &MainWindow::setImageForAcceptPage);

    // from pick images page
    connect(pickImagesPage, &PickImagesPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(pickImagesPage, &PickImagesPage::navigateToProjectPage, this, &MainWindow::navigateToProjectPage);
}


void MainWindow::showProjectionWindow()
{
    // if (imageProjectionWindow) {
    //     imageProjectionWindow->show();
    //     stackedWidget->setCurrentWidget(calibrationPage);
    // }
    // else{
    //     qDebug("In showProjectionWindow imageprojectionwindow is null");
    // }
}

void MainWindow::navigateToUserPage()
{
    stackedWidget->setCurrentWidget(userPage);
}

void MainWindow::navigateToFavoritesPage()
{
    stackedWidget->setCurrentWidget(favoritesPage);
}

void MainWindow::navigateToSettingsPage()
{
    stackedWidget->setCurrentWidget(settingsPage);
}

void MainWindow::navigateToCreatePage()
{
    stackedWidget->setCurrentWidget(createPage);
}

void MainWindow::navigateToCalibrationPage()
{
    // stackedWidget->setCurrentWidget(calibrationPage);
}

void MainWindow::navigateToSensitivityPage()
{
    // if (imageProjectionWindow) {
    //     sensitivityPage->setProjectionWindow(imageProjectionWindow);
    // } else {
    //     qDebug("Error: imageProjectionWindow is null in navigateToSensitivityPage");
    // }
    // stackedWidget->setCurrentWidget(sensitivityPage);
}

void MainWindow::navigateToTextVisionPage()
{
    // stackedWidget->setCurrentWidget(textVisionPage);
}

void MainWindow::navigateToPickImagesPage()
{
    // stackedWidget->setCurrentWidget(pickImagesPage);
}

void MainWindow::navigateToProjectPage()
{
    // stackedWidget->setCurrentWidget(projectPage);
}

// pictures

void MainWindow::setImageForAcceptPage(const QImage &image)
{
    // Assuming you have an AcceptPicturePage class
    // acceptPicturePage->setImage(image);
}

