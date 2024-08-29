#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    setupPages();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Create a QStackedWidget
    stackedWidget = new QStackedWidget(this);

    // Create a central widget for the main window
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(stackedWidget);

    // Set the central widget
    setCentralWidget(centralWidget);
}


void MainWindow::setupPages()
{
    // create instance of page
    createPage = new CreatePage(this);
    picturePage = new PicturePage(this);
    acceptPicturePage = new AcceptPicturePage(this);
    sensitivityPage = new SensitivityPage(this);
    textVisionPage = new TextVisionPage(this);
    pickImagesPage = new PickImagesPage(this);
    projectPage = new ProjectPage(this);

    // add each page
    stackedWidget->addWidget(createPage);
    stackedWidget->addWidget(picturePage);
    stackedWidget->addWidget(acceptPicturePage);
    stackedWidget->addWidget(sensitivityPage);
    stackedWidget->addWidget(textVisionPage);
    stackedWidget->addWidget(pickImagesPage);
    stackedWidget->addWidget(projectPage);

    // Start with the create page
    stackedWidget->setCurrentWidget(createPage);
}

void MainWindow::setupConnections()
{
    // from create page
    connect(createPage, &CreatePage::navigateToPicturePage, this, &MainWindow::navigateToPicturePage);
    // from picture page
    connect(picturePage, &PicturePage::navigateToAcceptPicturePage, this, &MainWindow::navigateToAcceptPicturePage);
    // from accept page
    connect(acceptPicturePage, &AcceptPicturePage::navigateToSensitivityPage, this, &MainWindow::navigateToSensitivityPage);
    connect(acceptPicturePage, &AcceptPicturePage::navigateToPicturePage, this, &MainWindow::navigateToPicturePage);
    // from sensitivity page
    connect(sensitivityPage, &SensitivityPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(sensitivityPage, &SensitivityPage::navigateToPicturePage, this, &MainWindow::navigateToPicturePage);
    // from text vision page
    connect(textVisionPage, &TextVisionPage::navigateToPickImagesPage, this, &MainWindow::navigateToPickImagesPage);
    // from pick images page
    connect(pickImagesPage, &PickImagesPage::navigateToTextVisionPage, this, &MainWindow::navigateToTextVisionPage);
    connect(pickImagesPage, &PickImagesPage::navigateToProjectPage, this, &MainWindow::navigateToProjectPage);
}

void MainWindow::navigateToPicturePage()
{
    stackedWidget->setCurrentWidget(picturePage);
}

void MainWindow::navigateToAcceptPicturePage()
{
    stackedWidget->setCurrentWidget(acceptPicturePage);
}

void MainWindow::navigateToSensitivityPage()
{
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


