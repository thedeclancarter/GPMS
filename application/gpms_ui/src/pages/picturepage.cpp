#include "picturepage.h"
#include "ui_picturepage.h"

PicturePage::PicturePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PicturePage)
{
    ui->setupUi(this);
    connect(ui->submitPictureButton, &QPushButton::clicked, this, &PicturePage::onSubmitPictureButtonClicked);
}

PicturePage::~PicturePage()
{
    delete ui;
}

void PicturePage::onSubmitPictureButtonClicked()
{
    emit navigateToAcceptPicturePage();
}
