#include "textvisionpage.h"
#include "ui_textvisionpage.h"

TextVisionPage::TextVisionPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextVisionPage)
{
    ui->setupUi(this);
    connect(ui->submitVisionButton, &QPushButton::clicked, this, &TextVisionPage::onSubmitButtonClicked);
}

TextVisionPage::~TextVisionPage()
{
    delete ui;
}

void TextVisionPage::onSubmitButtonClicked()
{
    emit navigateToPickImagesPage();
}
