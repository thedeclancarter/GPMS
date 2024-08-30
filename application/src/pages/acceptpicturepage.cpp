#include "acceptpicturepage.h"
#include "ui_acceptpicturepage.h"
#include <QGraphicsScene>


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
    if (!currentImage.isNull()) {
        emit navigateToSensitivityPage(currentImage);  // Make sure to pass the image
    } else {
        qDebug() << "No image to accept";
    }
}

void AcceptPicturePage::setImage(const QImage &image)
{
    qDebug() << "Received image in AcceptPicturePage";
    currentImage = image;

    // Display the image
    QPixmap pixmap = QPixmap::fromImage(currentImage);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
