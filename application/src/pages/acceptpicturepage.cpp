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
    emit navigateToSensitivityPage();
}

void AcceptPicturePage::setImage(const QImage &image)
{
    qDebug() << "Setting image in QGraphicsView";

    // Convert QImage to QPixmap
    QPixmap pixmap = QPixmap::fromImage(image);

    // Create a scene
    QGraphicsScene *scene = new QGraphicsScene(this);

    // Add the pixmap to the scene
    scene->addPixmap(pixmap);

    // Assuming imageWidget is a QGraphicsView, set the scene
    ui->graphicsView->setScene(scene);

    // Optionally, adjust the view settings
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio); // Fits the image to the view
}
