#include "calibrationpage.h"
#include "ui_calibrationpage.h"
#include "imageprojectionwindow.h"

CalibrationPage::CalibrationPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalibrationPage)
    , m_projectionWindow(projectionWindow)
{
    ui->setupUi(this);
    connect(ui->calibrateButton, &QPushButton::clicked, this, &CalibrationPage::onCalibrateButtonClicked);
}

void CalibrationPage::onCalibrateButtonClicked()
{
    qDebug("Going to sensitivity page");
    emit navigateToSensitivityPage();
}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}
