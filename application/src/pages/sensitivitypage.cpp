#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"
#include "imageprojectionwindow.h"

#include <QGraphicsScene>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>


SensitivityPage::SensitivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
{
    ui->setupUi(this);
    initializeUI();
    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);
    connect(ui->rejectSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onRejectButtonClicked);

    connect(lowerSlider, &QSlider::valueChanged, this, [this]() {
        applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
    });

    connect(upperSlider, &QSlider::valueChanged, this, [this]() {
        applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
    });
}

void SensitivityPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel());

    mainLayout->addWidget(createImageFrame(), 1);

    QFrame *sliderFrame = new QFrame(this);
    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderFrame);
    sliderFrame->setLayout(sliderLayout);

    lowerSlider = new QSlider(Qt::Horizontal, this);
    upperSlider = new QSlider(Qt::Horizontal, this);

    createSlider(lowerSlider);
    createSlider(upperSlider);

    sliderLayout->addWidget(lowerSlider);
    sliderLayout->addWidget(upperSlider);

    mainLayout->addWidget(sliderFrame);

    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);
}


void SensitivityPage::applyCannyEdgeDetection(int lowerThreshold, int upperThreshold)
{
    if (currentImage.isNull()) {
        qDebug() << "No image received in applyCannyEdgeDetection";
        return; // No image to process
    }

    // Convert QImage to cv::Mat
    cv::Mat mat = cv::Mat(currentImage.height(), currentImage.width(), CV_8UC4, const_cast<uchar*>(currentImage.bits()), currentImage.bytesPerLine()).clone();

    // Convert the image to grayscale
    cv::Mat grayMat;
    cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(grayMat, edges, lowerThreshold, upperThreshold);

    // Convert edges (cv::Mat) back to QImage
    QImage edgeImage = QImage(edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Grayscale8).copy();

    // Display the processed image
    QPixmap pixmap = QPixmap::fromImage(edgeImage);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setSceneRect(pixmap.rect());
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void SensitivityPage::setAcceptedImage(const QImage &image)
{
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

    // Apply Canny edge detection with default values
    applyCannyEdgeDetection(lowerSlider->value(), upperSlider->value());
}

QLabel* SensitivityPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Change the sensitivity of the outline", this);
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

QFrame* SensitivityPage::createImageFrame()
{
    QFrame *imageFrame = new QFrame(this);
    imageFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    imageFrame->setLineWidth(2);
    imageFrame->setStyleSheet("border-radius: 20px; background-color: #3E3E3E; border: 2px solid #3E3E3E;");

    QVBoxLayout *imageLayout = new QVBoxLayout(imageFrame);
    imageLayout->setContentsMargins(10, 10, 10, 10);
    imageLayout->addWidget(ui->graphicsView);
    ui->graphicsView->setStyleSheet("background: transparent; border: none;");

    imageFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return imageFrame;
}

QSlider* SensitivityPage::createSlider(QSlider* slider)
{
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 300);  // Adjust range as needed
    slider->setValue(150);     // Default value
    slider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #999999;"
        "    height: 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
        "    margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);"
        "    border: 1px solid #5c5c5c;"
        "    width: 18px;"
        "    margin: -2px 0;"
        "    border-radius: 9px;"
        "}"
        );
    slider->setFixedHeight(40);
    return slider;
}

QHBoxLayout* SensitivityPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->rejectSensitivityButton, "LET'S TRY AGAIN!", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->acceptSensitivityButton, "THIS LOOKS GOOD!", "#6FCD6F"));
    return buttonLayout;
}

QPushButton* SensitivityPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
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
    if (!currentImage.isNull()) {
        ImageProjectionWindow *projectionWindow = new ImageProjectionWindow(currentImage);
        projectionWindow->setAttribute(Qt::WA_DeleteOnClose); // Ensure the window is deleted when closed
        projectionWindow->show();
    }

    emit navigateToTextVisionPage();
}
