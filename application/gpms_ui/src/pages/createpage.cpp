#include "createpage.h"
#include "ui_createpage.h"

CreatePage::CreatePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreatePage)
{
    ui->setupUi(this);
    connect(ui->createButton, &QPushButton::clicked, this, &CreatePage::onCreateButtonClicked);
}

CreatePage::~CreatePage()
{
    delete ui;
}

void CreatePage::onCreateButtonClicked()
{
    emit navigateToPicturePage();
}
