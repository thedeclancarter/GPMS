#include "acceptpicturepage.h"
#include "ui_acceptpicturepage.h"
#include <QGraphicsScene>
#include <QVBoxLayout>


AcceptPicturePage::AcceptPicturePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AcceptPicturePage)
{
    ui->setupUi(this);
    initializeUI();

    connect(ui->AcceptPictureButton, &QPushButton::clicked, this, &AcceptPicturePage::onAcceptButtonClicked);
    connect(ui->RejectPictureButton, &QPushButton::clicked, this, &AcceptPicturePage::onRejectButtonClicked);

}

void AcceptPicturePage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel());
    mainLayout->addWidget(createImageFrame(), 1);
    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);
}

QLabel* AcceptPicturePage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Accept or reject your picture", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"
        "border-radius: 20px;"
        "padding: 15px 20px;"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    return titleLabel;
}

QFrame* AcceptPicturePage::createImageFrame()
{
    QFrame *imageFrame = new QFrame(this);
    imageFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    imageFrame->setLineWidth(2);
    imageFrame->setStyleSheet("border-radius: 20px; background-color: #3E3E3E; border: 2px solid #3E3E3E;");

    QVBoxLayout *imageLayout = new QVBoxLayout(imageFrame);
    imageLayout->setContentsMargins(10, 10, 10, 10);  // Reduce margins
    imageLayout->addWidget(ui->graphicsView);
    ui->graphicsView->setStyleSheet("background: transparent; border: none;");

    // // Set a size policy that allows the frame to expand
    imageFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return imageFrame;
}

QHBoxLayout* AcceptPicturePage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->RejectPictureButton, "LETS TRY AGAIN!", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->AcceptPictureButton, "THIS LOOKS GOOD!", "#6FCD6F"));
    return buttonLayout;
}

QPushButton* AcceptPicturePage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    button->setText(text);
    button->setFixedSize(200, 50);
    button->setStyleSheet(QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: white;"
        "   border-radius: 25px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: darker(%1, 120%);"
        "}"
        "QPushButton:pressed {"
        "   background-color: darker(%1, 140%);"
        "}"
    ).arg(bgColor));
    return button;
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
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setSceneRect(pixmap.rect());
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
