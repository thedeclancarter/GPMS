#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"

SensitivityPage::SensitivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
{
    ui->setupUi(this);
    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);
    connect(ui->rejectSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onRejectButtonClicked);
}

SensitivityPage::~SensitivityPage()
{
    delete ui;
}

void SensitivityPage::onRejectButtonClicked()
{
    emit navigateToPicturePage();
}

void SensitivityPage::onAcceptButtonClicked()
{
    emit navigateToTextVisionPage();
}
