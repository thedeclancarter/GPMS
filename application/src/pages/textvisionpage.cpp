#include "textvisionpage.h"
#include "ui_textvisionpage.h"

#include <QVBoxLayout>


TextVisionPage::TextVisionPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextVisionPage)
    , m_isRealistic(true)
{
    setupUI();
    setupLayouts();
    setupStyleSheets();
    setupConnections();
    connect(ui->submitVisionButton, &QPushButton::clicked, this, &TextVisionPage::onSubmitButtonClicked);
}

void TextVisionPage::setupUI()
{
    m_title = createTitleLabel();
    m_realisticButton = new QPushButton("Realistic", this);
    m_animatedButton = new QPushButton("Animated", this);
    updateButtonStyles();
    m_visionInput = new QTextEdit(this);
    m_visionInput->setFixedSize(600, 200);
    m_submitButton = createSubmitButton();
}

QLabel* TextVisionPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Tell us your vision in 1-2 sentences", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"  // Same as webcam frame background
        "border-radius: 20px;"
        "padding: 15px 20px;"  // Vertical and horizontal padding
        );
    titleLabel->setFixedSize(800,60);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    return titleLabel;
}

void TextVisionPage::setupLayouts()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *optionsLayout = new QHBoxLayout();

    optionsLayout->addWidget(m_realisticButton);
    optionsLayout->addWidget(m_animatedButton);

    mainLayout->setSpacing(15);

    mainLayout->addWidget(m_title, 0, Qt::AlignHCenter);
    mainLayout->addLayout(optionsLayout);
    mainLayout->addWidget(m_visionInput, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_submitButton, 0, Qt::AlignHCenter);

    setLayout(mainLayout);
}

void TextVisionPage::setupStyleSheets()
{
    m_visionInput->setStyleSheet("color: white; background-color: #2E2E2E; border-radius: 10px; padding: 20px");
}

QPushButton* TextVisionPage::createSubmitButton()
{
    QPushButton* submit_button = new QPushButton("+ SUBMIT", this);
    submit_button->setFixedSize(120, 40);
    submit_button->setStyleSheet(
        "QPushButton {"
        "   background-color: #6F81CD;"
        "   color: white;"
        "   border-radius: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4A5A9F;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5D5D5D;"
        "}"
        );
    return submit_button;
}

void TextVisionPage::setupConnections()
{
    connect(m_submitButton, &QPushButton::clicked, this, &TextVisionPage::onSubmitButtonClicked);
    connect(m_realisticButton, &QPushButton::clicked, this, &TextVisionPage::onRealisticButtonClicked);
    connect(m_animatedButton, &QPushButton::clicked, this, &TextVisionPage::onAnimatedButtonClicked);
}

TextVisionPage::~TextVisionPage()
{
    delete ui;
}

void TextVisionPage::updateButtonStyles()
{
    m_realisticButton->setStyleSheet(m_isRealistic ? SELECTED_STYLE : UNSELECTED_STYLE);
    m_animatedButton->setStyleSheet(m_isRealistic ? UNSELECTED_STYLE : SELECTED_STYLE);
}

void TextVisionPage::onSubmitButtonClicked()
{
    m_visionText = m_visionInput->toPlainText();
    emit navigateToPickImagesPage();
}

void TextVisionPage::onRealisticButtonClicked()
{
    m_isRealistic = true;
    updateButtonStyles();
}

void TextVisionPage::onAnimatedButtonClicked()
{
    m_isRealistic = false;
    updateButtonStyles();
}
