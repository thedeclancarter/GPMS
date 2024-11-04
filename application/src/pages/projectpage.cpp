#include "projectpage.h"
#include "ui_projectpage.h"
#include "imageprojectionwindow.h"

#include <QVBoxLayout>
#include <opencv2/imgcodecs.hpp>

ProjectPage::ProjectPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectPage)
    , m_imageLabel(nullptr)
{
    ui->setupUi(this);
    initializeUI();
}

ProjectPage::~ProjectPage()
{
    delete ui;
}

void ProjectPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel(), 0, Qt::AlignHCenter);

    mainLayout->addWidget(createImageFrame(), 1, Qt::AlignHCenter);

    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);

    connect(ui->doneButton, &QPushButton::clicked, this, &ProjectPage::onDoneButtonClicked);
    connect(ui->rejectButton, &QPushButton::clicked, this, &ProjectPage::onRejectButtonClicked);
}

void ProjectPage::setSelectedImage(const cv::Mat& mat)
{
    if (mat.empty() || mat.type() != CV_8UC3) {
        qDebug("Invalid image format. Expected non-empty CV_8UC3.");
        return;
    }

    // Convert cv::Mat to QImage without copying data
    QImage qimg(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

    // Scale the image to fit the label while maintaining aspect ratio
    m_imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QLabel* ProjectPage::createTitleLabel()
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

QFrame* ProjectPage::createImageFrame()
{
    QFrame *imageFrame = new QFrame(this);
    imageFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    imageFrame->setLineWidth(2);
    imageFrame->setStyleSheet("border-radius: 10px; background-color: #2E2E2E;");
    imageFrame->setFixedWidth(800);
    imageFrame->setContentsMargins(3, 3, 3, 3);

    QVBoxLayout *cameraLayout = new QVBoxLayout(imageFrame);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cameraLayout->addWidget(m_imageLabel);

    return imageFrame;
}


QHBoxLayout* ProjectPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->rejectButton, "REVISE VISION", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->doneButton, "FINISHED PROJECTING", "#BB64C7"));
    return buttonLayout;
}

QPushButton* ProjectPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
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

void ProjectPage::onRejectButtonClicked()
{
    emit navigateToPickImagesPage();
}

void ProjectPage::onDoneButtonClicked()
{
    emit requestImageRefresh();
    emit navigateToCreatePage();
}
