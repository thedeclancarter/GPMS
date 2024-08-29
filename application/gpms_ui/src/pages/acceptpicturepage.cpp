#include "acceptpicturepage.h"
#include "ui_acceptpicturepage.h"

AcceptPicturePage::AcceptPicturePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AcceptPicturePage)
{
    ui->setupUi(this);
    connect(ui->AcceptPictureButton, &QPushButton::clicked, this, &AcceptPicturePage::onAcceptButtonClicked);
    connect(ui->RejectPictureButton, &QPushButton::clicked, this, &AcceptPicturePage::onRejectButtonClicked);
}

AcceptPicturePage::~AcceptPicturePage()
{
    delete ui;
}

void AcceptPicturePage::onRejectButtonClicked()
{
    emit navigateToPicturePage();
}

void AcceptPicturePage::onAcceptButtonClicked()
{
    emit navigateToSensitivityPage();
}
