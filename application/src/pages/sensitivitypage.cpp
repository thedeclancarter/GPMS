#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"

#include <QGraphicsScene>


SensitivityPage::SensitivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
{
    ui->setupUi(this);
    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);
    connect(ui->rejectSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onRejectButtonClicked);
}


void SensitivityPage::setAcceptedImage(const QImage &image)
{
    currentImage = image;
    updateImageDisplay();
}

void SensitivityPage::updateImageDisplay()
{
    if (!currentImage.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(currentImage);
        QGraphicsScene *scene = new QGraphicsScene(this);
        scene->addPixmap(pixmap);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    } else {
        qDebug() << "No image to display";
    }
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
