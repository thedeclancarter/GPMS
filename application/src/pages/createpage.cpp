#include "createpage.h"
#include "ui_createpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
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
    mainLayout->setContentsMargins(35, 10, 35, 10);

    setupHeader(mainLayout);

    // Create a grid layout for the main content
    QGridLayout *contentLayout = new QGridLayout();
    contentLayout->setColumnStretch(0, 1);
    contentLayout->setColumnStretch(1, 1);
    mainLayout->addLayout(contentLayout);

    // Add steps and image preview sections side by side
    setupLeftColumn(contentLayout, 0, 0);
    setupRightColumn(contentLayout, 0, 1);

    setupCreateButton(mainLayout);
}

void CreatePage::setupHeader(QVBoxLayout *layout)
{

    QLabel *titleLabel = new QLabel("MAKE YOUR VISION COME TO LIFE", this);
    titleLabel->setStyleSheet("font-size: 38px; font-weight: bold; color: white;");
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QWidget *headerContainer = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerContainer);
    headerLayout->setContentsMargins(30, 10, 30, 10);
    headerLayout->addWidget(titleLabel);


    headerContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px;");
    headerContainer->setFixedWidth(800);

    // Create a wrapper widget to center the header container
    QWidget *centeringWrapper = new QWidget(this);
    QHBoxLayout *centeringLayout = new QHBoxLayout(centeringWrapper);
    centeringLayout->addWidget(headerContainer, 0, Qt::AlignCenter);

    layout->addWidget(centeringWrapper);

    QLabel *descriptionLabel = new QLabel("Transform your space with GPMS, an AI-powered projection mapping system to create stunning visuals based on your environment and imagination.", this);
    descriptionLabel->setStyleSheet("font-size: 16px; color: #a0aec0; margin: 0px; padding: 0px;");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    QWidget *descriptionWrapper = new QWidget(this);
    QHBoxLayout *descriptionLayout = new QHBoxLayout(descriptionWrapper);
    descriptionLayout->setContentsMargins(0, 0, 0, 0);  // Remove margins
    descriptionLayout->setSpacing(0);  // Remove spacing
    descriptionLayout->addWidget(descriptionLabel);
    descriptionWrapper->setFixedWidth(800);

    layout->addWidget(descriptionWrapper, 0, Qt::AlignCenter);
}

void CreatePage::setupLeftColumn(QGridLayout *layout, int row, int column)
{
    QWidget *stepsContainer = new QWidget(this);
    QVBoxLayout *stepsLayout = new QVBoxLayout(stepsContainer);
    stepsLayout->setSpacing(10);  // Add some spacing between items
    stepsLayout->setContentsMargins(20, 20, 20, 20);  // Keep padding inside the container

    QLabel *stepsHeader = new QLabel("3 Easy Steps", stepsContainer);
    stepsHeader->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    stepsLayout->addWidget(stepsHeader);

    stepsLayout->addSpacing(10);  // Add space after the header

    QStringList steps = {"Calibrate the camera", "Input your vision", "Choose your image"};
    QStringList descriptions = {
        "Optimize the system to accurately capture your environment.",
        "Input your creative concept and desired outcome.",
        "Select an image to project."
    };

    for (int i = 0; i < steps.size(); ++i) {
        QHBoxLayout *stepLayout = new QHBoxLayout();
        stepLayout->setContentsMargins(15, 0, 0, 0);  // Add left indentation

        QVBoxLayout *stepContentLayout = new QVBoxLayout();

        QLabel *stepLabel = new QLabel(QString("%1) %2").arg(i+1).arg(steps[i]));
        stepLabel->setStyleSheet("font-size: 18px; color: white; font-weight: bold;");
        stepContentLayout->addWidget(stepLabel);

        QLabel *descriptionLabel = new QLabel(descriptions[i]);
        descriptionLabel->setStyleSheet("font-size: 14px; color: #a0aec0;");
        descriptionLabel->setWordWrap(true);
        stepContentLayout->addWidget(descriptionLabel);

        stepLayout->addLayout(stepContentLayout);
        stepsLayout->addLayout(stepLayout);

        if (i < steps.size() - 1) {
            stepsLayout->addSpacing(10);  // Add space between steps
        }
    }

    stepsContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px;");
    stepsContainer->setFixedHeight(275);  // Set both width and height
    layout->addWidget(stepsContainer, row, column);
}

void CreatePage::setupRightColumn(QGridLayout *layout, int row, int column)
{
    QWidget *previewContainer = new QWidget(this);
    previewContainer->setFixedHeight(275);  // Match the height of the steps container
    previewContainer->setStyleSheet("background-color: #2C2C2E; border-radius: 15px;");

    QVBoxLayout *previewLayout = new QVBoxLayout(previewContainer);
    QLabel *previewLabel = new QLabel("Preview of Your Projection Will Appear Here", previewContainer);
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet("color: #a0aec0;");
    previewLayout->addWidget(previewLabel);

    layout->addWidget(previewContainer, row, column);
}

void CreatePage::setupCreateButton(QVBoxLayout *layout)
{
    createButton = new QPushButton("CREATE YOUR VISION", this);
    createButton->setCursor(Qt::PointingHandCursor);
    createButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #6f81cd;"
        "   color: white;"
        "   border-radius: 25px;"
        "   padding: 15px 30px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #6F81CD;"
        "}"
        );
    createButton->setFixedSize(300, 60);  // Adjust size as needed

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(createButton);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 5);
    createButton->setGraphicsEffect(shadowEffect);

    layout->addWidget(createButton, 0, Qt::AlignCenter);
    createButton->setCursor(Qt::PointingHandCursor);
}

void CreatePage::setupConnections()
{
    if (createButton) {
        connect(createButton, &QPushButton::clicked, this, &CreatePage::onCreateButtonClicked);
    }
}

void CreatePage::onCreateButtonClicked()
{
    emit navigateToCalibrationPage();
}
