#include "pickimagespage.h"
#include "ui_pickimagespage.h"

PickImagesPage::PickImagesPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PickImagesPage)
{
    ui->setupUi(this);
    connect(ui->selectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onAcceptButtonClicked);
    connect(ui->rejectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onRejectButtonClicked);
}

PickImagesPage::~PickImagesPage()
{
    delete ui;
}

void PickImagesPage::onRejectButtonClicked()
{
    emit navigateToTextVisionPage();
}

void PickImagesPage::onAcceptButtonClicked()
{
    emit navigateToProjectPage();
}
