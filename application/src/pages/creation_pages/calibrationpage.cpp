#include "calibrationpage.h"
#include "ui_calibrationpage.h"

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalibrationPage)
{
    ui->setupUi(this);
}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}
