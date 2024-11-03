#include "pickimagespage.h"
#include "ui_pickimagespage.h"
#include "../utils/image_utils.h"
#include "imageprojectionwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>


// ClickableFrame implementation
ClickableFrame::ClickableFrame(QWidget *parent) : QFrame(parent)
    ,m_selected(false)
    ,m_imageLabel(nullptr)
{
    setLayout(new QVBoxLayout(this));
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setContentsMargins(5, 5, 5, 5);
    layout()->addWidget(m_imageLabel);
    updateStyle();

    // Add a layout to the frame for displaying an image
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout); // Ensure layout is applied to frame
}


void ClickableFrame::setSelected(bool selected)
{
    m_selected = selected;
    updateStyle();
}

bool ClickableFrame::isSelected() const
{
    return m_selected;
}

void ClickableFrame::setImage(const cv::Mat& mat)
{
    if (mat.type() != CV_8UC3) {
        qDebug() << "Invalid image format. Expected CV_8UC3.";
        return;
    }
    m_image = mat.clone();

    // Convert cv::Mat to QImage without copying data
    QImage qimg(m_image.data, m_image.cols, m_image.rows, m_image.step, QImage::Format_RGB888);
    m_imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

cv::Mat ClickableFrame::getImage() const
{
    return m_image;
}

void ClickableFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setSelected(!m_selected);
        emit clicked();
    }
    QFrame::mousePressEvent(event);
}

void ClickableFrame::updateStyle()
{
    QString style = QString(
                        "ClickableFrame {"
                        "   border-radius: 20px;"
                        "   background-color: #3E3E3E;"
                        "   border: %1px solid %2;"
                        "}")
                        .arg(m_selected ? "2" : "1", m_selected ? "#FFD700" : "#3E3E3E");

    setStyleSheet(style);
}

void PickImagesPage::handleImageResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
    {
        qDebug() << "No reply received";
        return;
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray imageData = reply->readAll();

        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

        if (contentType.startsWith("image"))
        {
            QPixmap pixmap;
            if (pixmap.loadFromData(imageData))
            {

                // Iterate through the frames and set pixmap for the first available frame
                for (int i = 0; i < m_imageFrames.size(); ++i)
                {
                    ClickableFrame *frame = m_imageFrames.at(i);

                    // Check if the frame has any children (e.g., QLabel for image)
                    if (frame->layout()->isEmpty())
                    {
                        QLabel *imageLabel = new QLabel(frame);

                        imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                        imageLabel->setAlignment(Qt::AlignCenter);
                        imageLabel->setScaledContents(true);

                        // Scale the pixmap to fit the frame
                        imageLabel->setPixmap(pixmap);

                        // Add the QLabel to the frame's layout
                        frame->layout()->addWidget(imageLabel);

                        // Ensure layout updates
                        frame->update();
                        frame->adjustSize();

                        break; // Break after filling the first available frame
                    }
                }
            }
            else
            {
                qDebug() << "Failed to load pixmap from data";
            }
        }
        else
        {
            qDebug() << "Received content is not an image!";
        }
    }
    else
    {
        qDebug() << "Network reply error: " << reply->errorString();
    }

    reply->deleteLater();
}

void PickImagesPage::fetchRandomImages()
{
    qDebug() << "Fetching random images...";

    QNetworkRequest request1(QUrl("https://picsum.photos/200/150"));
    request1.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    QNetworkReply *reply1 = m_networkManager->get(request1);

    QNetworkRequest request2(QUrl("https://picsum.photos/200/150"));
    request2.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    QNetworkReply *reply2 = m_networkManager->get(request2);

    // Make sure to connect each reply to the image handler
    connect(reply1, &QNetworkReply::finished, this, &PickImagesPage::handleImageResponse);
    connect(reply2, &QNetworkReply::finished, this, &PickImagesPage::handleImageResponse);
}


void PickImagesPage::fetchRandomImages(int numImages)
{
    for (int i = 0; i < numImages; ++i) {
        QNetworkRequest request(QUrl("https://picsum.photos/1280/720"));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
        QNetworkReply *reply = m_networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            handleImageResponse(reply);
        });
    }
}




// Pick Images Page
PickImagesPage::PickImagesPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PickImagesPage)
    , m_projectionWindow(projectionWindow)
    , m_selectedFrame(nullptr)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    initializeUI();

    QCoreApplication::addLibraryPath("C:/Program Files/openssl-1.1/x64/bin");
    // connect buttons
    connect(ui->selectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onAcceptButtonClicked);
    connect(ui->rejectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onRejectButtonClicked);
    connect(ui->retakePhotoButton, &QPushButton::clicked, this, &PickImagesPage::onRetakePhotoButtonClicked);

    fetchRandomImages(2);
}

PickImagesPage::~PickImagesPage()
{
    delete ui;
}

void PickImagesPage::clearSelections(){
    if (m_selectedFrame){
        m_selectedFrame->setSelected(false);
    }
}

cv::Mat PickImagesPage::getSelectedImage() const
{
    return m_selectedFrame ? m_selectedFrame->getImage() : cv::Mat();
}

void PickImagesPage::handleImageResponse(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray imageData = reply->readAll();
        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

        if (contentType.startsWith("image")) {
            QImage qimg;
            if (qimg.loadFromData(imageData)) {
                // Ensure the image is in RGB format
                if (qimg.format() != QImage::Format_RGB888) {
                    qimg = qimg.convertToFormat(QImage::Format_RGB888);
                }
                cv::Mat mat = ImageUtils::qimage_to_mat(qimg);
                for (int i = 0; i < m_imageFrames.size(); ++i) {
                    ClickableFrame* frame = m_imageFrames.at(i);
                    if (frame->getImage().empty()) {
                        frame->setImage(mat);
                        break;
                    }
                }
            }
        }
    }
    reply->deleteLater();
}


void PickImagesPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel(), 0, Qt::AlignHCenter);
    mainLayout->addWidget(createImagesGrid(), 1, Qt::AlignHCenter);
    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);
}

QLabel *PickImagesPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Pick The Images You like Best", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"
        "border-radius: 20px;"
        "padding: 15px 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    titleLabel->setFixedWidth(800);
    return titleLabel;
}

QFrame *PickImagesPage::createImagesGrid()
{
    QFrame *gridFrame = new QFrame(this);
    gridFrame->setFrameStyle(QFrame::NoFrame);
    gridFrame->setStyleSheet("background-color: transparent;");
    gridFrame->setFixedWidth(900);

    QGridLayout *gridLayout = new QGridLayout(gridFrame);
    gridLayout->setSpacing(10);

    for (int i = 0; i < 2; ++i)
    {
        ClickableFrame *imageFrame = new ClickableFrame(this);
        imageFrame->setFixedSize(400, 350);
        connect(imageFrame, &ClickableFrame::clicked, this, [this, imageFrame]() {
            updateSelectedImages(imageFrame);
        });
        gridLayout->addWidget(imageFrame, i / 2, i % 2);
        m_imageFrames.append(imageFrame);
    }

    gridFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return gridFrame;
}

void PickImagesPage::updateSelectedImages(ClickableFrame *clickedFrame)
{
    // if current fram isnt the clicked frame
    if (m_selectedFrame && m_selectedFrame != clickedFrame) {
        m_selectedFrame->setSelected(false);  // Deselect the previous frame
    }

    // if clicked frame is selected
    if (clickedFrame->isSelected()) {
        m_selectedFrame = clickedFrame;  // Update to the newly selected frame
        cv::Mat finalFrame= clickedFrame->getImage();
        cv::Mat BGR_image;
        cv::cvtColor(finalFrame, BGR_image, cv::COLOR_RGB2BGR);
        m_projectionWindow->setFinalFrame(BGR_image); // set final frame w this image
        m_projectionWindow->setProjectionState(ImageProjectionWindow::projectionState::IMAGE);
        // change proj to show new image
    } else {
        m_selectedFrame = nullptr;  // Clear the selection if the frame was deselected
        // change proj to show rainbow
        m_projectionWindow->setProjectionState(ImageProjectionWindow::projectionState::RAINBOW_EDGE);
    }

    // Enable or disable the select button based on whether any frame is selected
    ui->selectImagesButton->setEnabled(m_selectedFrame != nullptr);
}

QHBoxLayout *PickImagesPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->retakePhotoButton, "RETAKE PHOTO", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->rejectImagesButton, "REVISE MY VISION", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->selectImagesButton, "CHOOSE PICTURE", "#BB64c7"));
    ui->selectImagesButton->setEnabled(false); // Initially disabled
    return buttonLayout;
}

QPushButton *PickImagesPage::styleButton(QPushButton *button, const QString &text, const QString &bgColor)
{
    // Define hover colors as 30% darker versions of the original colors
    QString hoverColor;
    if (bgColor == "#CD6F6F")
    {
        hoverColor = "#9F5D5D"; // 30% darker version of #CD6F6F
    }
    else if (bgColor == "#BB64c7")
    {
        hoverColor = "#83468B"; // 30% darker version of #6FCD6F
    }
    else
    {
        hoverColor = "#FFD700"; // Fallback hover color (for other cases)
    }

    button->setText(text);
    button->setFixedHeight(50);
    button->setFixedWidth(250);
    button->setStyleSheet(QString(
                              "QPushButton {"
                              "   background-color: %1;" // Original color
                              "   color: black;"
                              "   border-radius: 25px;"
                              "   font-weight: bold;"
                              "   font-size: 16px;"
                              "   padding: 0 20px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: %2;" // 30% darker color for hover
                              "}"
                              "QPushButton:pressed {"
                              "   background-color: darker(%1, 140%);" // Ensure the pressed color is darker
                              "}"
                              "QPushButton:disabled {"
                              "   background-color: #808080;" // Disabled state color
                              "}")
                              .arg(bgColor)
                              .arg(hoverColor));

    // Set the hand cursor when hovering over the button
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

void PickImagesPage::onRejectButtonClicked()
{
    updateSelectedImages(m_selectedFrame);
    clearImages();
    fetchRandomImages(2);
    emit navigateToTextVisionPage();
}

void PickImagesPage::onRetakePhotoButtonClicked()
{
    updateSelectedImages(m_selectedFrame);
    clearImages();
    fetchRandomImages(2);
    emit navigateToSensitivityPage();
}

void PickImagesPage::onAcceptButtonClicked()
{
    if (!m_selectedFrame)
    {
        qDebug("No frame selected");
        return;
    }

    cv::Mat selectedImage = m_selectedFrame->getImage();

    emit navigateToProjectPage(selectedImage);
}

void PickImagesPage::clearImages()
{
    // Iterate through the frames and remove all child widgets (i.e., the QLabel holding the images)
    for (int i = 0; i < m_imageFrames.size(); ++i)
    {
        QLayout *layout = m_imageFrames[i]->layout();
        while (QLayoutItem *item = layout->takeAt(0))
        {
            delete item->widget(); // Delete the QLabel
        }
    }
}

void PickImagesPage::refreshImages()
{
    clearImages();
    fetchRandomImages(2);
}

void PickImagesPage::clearImages()
{
    // Iterate through the frames and remove all child widgets (i.e., the QLabel holding the images)
    for (int i = 0; i < m_imageFrames.size(); ++i)
    {
        QLayout *layout = m_imageFrames[i]->layout();
        while (QLayoutItem *item = layout->takeAt(0))
        {
            delete item->widget(); // Delete the QLabel
        }
    }
}

void PickImagesPage::refreshImages()
{
    clearImages();
    fetchRandomImages(2);
}
