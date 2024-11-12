#include "sensitivitypage.h"
#include "ui_sensitivitypage.h"
#include "imageprojectionwindow.h"

#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QtMultimedia/qcameraimagecapture.h>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QCameraInfo>


SensitivityPage::SensitivityPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SensitivityPage)
    , m_projectionWindow(projectionWindow)
    , m_imageLabel(nullptr)
    , lowerSlider(nullptr)
    , upperSlider(nullptr)
    , timer(nullptr)
    , m_camera(nullptr)
    , m_viewfinder(nullptr)
    , m_imageCapture(nullptr)
    , m_lastFrame(nullptr)
{
    ui->setupUi(this);
    init();

    connect(lowerSlider, &QSlider::valueChanged, this, &SensitivityPage::updateSensitivity);
    connect(upperSlider, &QSlider::valueChanged, this, &SensitivityPage::updateSensitivity);
}

void SensitivityPage::init()
{
    initializeUI();
}

void SensitivityPage::resetSensitivitySliders(){
    lowerSlider->setValue(150);
    upperSlider->setValue(150);
}

void SensitivityPage::setProjectionWindow(ImageProjectionWindow *projectionWindow)
{
    if (projectionWindow){
        m_projectionWindow = projectionWindow;
        qDebug("Setting projection window");
    }
    else{
        qDebug("Projection window is null");
    }

}

void SensitivityPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel(), 0, Qt::AlignHCenter);

    mainLayout->addWidget(createImageFrame(), 1, Qt::AlignHCenter);

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

    connect(ui->acceptSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onAcceptButtonClicked);
    connect(ui->rejectSensitivityButton, &QPushButton::clicked, this, &SensitivityPage::onRejectButtonClicked);
}

QLabel* SensitivityPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Change The Sensitivity of the Outline", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"
        "border-radius: 20px;"
        "padding: 15px 20px;"
        );
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    titleLabel->setFixedWidth(800);
    titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    return titleLabel;
}

QFrame* SensitivityPage::createImageFrame()
{
    // setupCamera();
    QFrame *cameraFrame = new QFrame(this);
    cameraFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    cameraFrame->setLineWidth(2);
    cameraFrame->setStyleSheet("border-radius: 10px; background-color: #2E2E2E;");
    cameraFrame->setFixedWidth(800);


    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraFrame);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cameraLayout->addWidget(m_imageLabel);
    return cameraFrame;
}

QSlider* SensitivityPage::createSlider(QSlider* slider)
{
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 300);  // Adjust range as needed
    slider->setValue(150);     // Default value
    slider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    height: 8px;"
        "    background-color: #FFF9C4;"
        "    margin: 2px 0;"
        "    border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background-color: #FFEB72;"
        "    border: none;"
        "    width: 18px;"
        "    height: 18px;"
        "    margin: -5px 0;"  // Centers the handle vertically with the groove
        "    border-radius: 9px;"
        "}"
        );
    slider->setFixedHeight(40);
    return slider;
}



QHBoxLayout* SensitivityPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->rejectSensitivityButton, "LET'S TRY AGAIN", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->acceptSensitivityButton, "THIS LOOKS GOOD!", "#BB64C7"));
    return buttonLayout;
}

QPushButton* SensitivityPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    // Compute the darker color by applying a 30% reduction
    QString darkerColor;
    if (bgColor == "#CD6F6F") {
        darkerColor = "#8B4D4D";  // 30% darker color for red
    } else if (bgColor == "#BB64C7") {
        darkerColor = "#83468B";  // 30% darker color for purple
    } else {
        darkerColor = "#4A5A9F";  // Default color (if none of the above match)
    }

    button->setText(text);
    button->setFixedSize(200, 50);
    button->setStyleSheet(QString(
                              "QPushButton {"
                              "   background-color: %1;"  // Original color
                              "   color: black;"
                              "   border-radius: 25px;"
                              "   font-weight: bold;"
                              "   font-size: 16px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: %2;"  // 30% darker color for hover
                              "}"
                              "QPushButton:pressed {"
                              "   background-color: %3;"  // 30% darker color for pressed
                              "}"
                              ).arg(bgColor).arg(darkerColor).arg(darkerColor));

    // Set the hand cursor when hovering over the button
    button->setCursor(Qt::PointingHandCursor);
    return button;
}


SensitivityPage::~SensitivityPage()
{
    if (timer) {
        timer->stop();
    }
    delete ui;
}

void SensitivityPage::onRejectButtonClicked()
{
    // endCaptureTimer();
    emit navigateToCalibrationPage();
}

void SensitivityPage::onAcceptButtonClicked()
{
    // endCaptureTimer();
    emit navigateToTextVisionPage(lowerSlider->value(), upperSlider->value());
}

bool SensitivityPage::checkCameraAvailability()
{
    return !QCameraInfo::availableCameras().isEmpty();
}

void SensitivityPage::updateSensitivity()
{
    int lowerValue = lowerSlider->value();
    int upperValue = upperSlider->value();

    if (m_projectionWindow) {
        m_projectionWindow->setSensitivity(lowerValue, upperValue);

        // Retrieve the current image from the projection window
        QImage image = m_projectionWindow->getCurrentImage();

        if (!image.isNull()) {
            // Update m_imageLabel with the new image
            m_imageLabel->setPixmap(QPixmap::fromImage(image).scaled(
                m_imageLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));
        } else {
            qDebug() << "Projection window returned a null image.";
        }
    } else {
        qDebug() << "Projection window is not set.";
    }
}
