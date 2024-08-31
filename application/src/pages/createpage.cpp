#include "createpage.h"
#include "ui_createpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

CreatePage::CreatePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreatePage)
    , createButton(nullptr)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

CreatePage::~CreatePage()
{
    delete ui;
}

void CreatePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    setupHeader(mainLayout);

    // Create a horizontal layout for the main content
    QHBoxLayout *contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);

    // Add steps and image preview sections side by side
    setupStepsSection(contentLayout);
    setupImagePreviewSection(contentLayout);

    setupCreateButton(mainLayout);
}

void CreatePage::setupHeader(QVBoxLayout *layout)
{
    QLabel *headerLabel = new QLabel("MAKE YOUR VISION COME TO LIFE", this);
    headerLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");

    QWidget *headerContainer = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerContainer);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->addWidget(headerLabel);
    headerContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px; padding: 10px;");

    layout->addWidget(headerContainer);
}

void CreatePage::setupStepsSection(QHBoxLayout *layout)
{
    QWidget *stepsContainer = new QWidget(this);
    QVBoxLayout *stepsLayout = new QVBoxLayout(stepsContainer);

    QLabel *stepsHeader = new QLabel("3 easy steps", stepsContainer);
    stepsHeader->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
    stepsLayout->addWidget(stepsHeader);

    QStringList steps = {"1. Take a picture & calibrate", "2. Input your vision", "3. Choose your image"};
    for (const QString &step : steps) {
        QLabel *stepLabel = new QLabel(step, stepsContainer);
        stepLabel->setStyleSheet("font-size: 16px; color: white;");
        stepsLayout->addWidget(stepLabel);
    }

    stepsContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px; padding: 15px;");
    stepsContainer->setFixedWidth(300);  // Adjust width as needed
    layout->addWidget(stepsContainer);
}

void CreatePage::setupImagePreviewSection(QHBoxLayout *layout)
{
    QWidget *previewContainer = new QWidget(this);
    previewContainer->setFixedSize(300, 200);  // Adjust size as needed
    previewContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px;");
    layout->addWidget(previewContainer);
}

void CreatePage::setupCreateButton(QVBoxLayout *layout)
{
    createButton = new QPushButton("+ CREATE YOUR VISION", this);
    createButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4A5A9F;"
        "   color: white;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #6F81CD;"
        "}"
        );
    createButton->setFixedSize(250, 50);  // Adjust size as needed

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(createButton);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 5);
    createButton->setGraphicsEffect(shadowEffect);

    layout->addWidget(createButton, 0, Qt::AlignCenter);
}

void CreatePage::setupConnections()
{
    if (createButton) {
        connect(createButton, &QPushButton::clicked, this, &CreatePage::onCreateButtonClicked);
    }
}

void CreatePage::onCreateButtonClicked()
{
    emit navigateToPicturePage();
}
