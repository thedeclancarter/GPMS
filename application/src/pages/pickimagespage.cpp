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
#include <QTimer>
#include <QUrlQuery>


// ClickableFrame implementation
ClickableFrame::ClickableFrame(QWidget *parent) : QFrame(parent)
    ,m_selected(false)
    ,m_imageLabel(nullptr)
    ,m_image(cv::Mat())
{
    setLayout(new QVBoxLayout(this));
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setContentsMargins(5, 5, 5, 5);
    layout()->addWidget(m_imageLabel);
    updateStyle();
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




// Pick Images Page
PickImagesPage::PickImagesPage(ImageProjectionWindow *projectionWindow, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PickImagesPage)
    , m_projectionWindow(projectionWindow)
    , m_selectedFrame(nullptr)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_prompt("")
    , m_isRealistic(false)
    , m_lowThreshold(0.0)
    , m_highThreshold(1.0)
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


QString PickImagesPage::getApiKey() {
    QFile file("../../../env");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open .env file";
        return QString();
    }

    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if (line.startsWith("api_key=")) {
            return line.mid(8); // Removes "api_key=" from the start
        }
    }
    return QString();
}

void PickImagesPage::fetchRandomImages(int numImages)
{
    if (numImages <= 0) {
        qDebug("Invalid number of images requested");
        return;
    }

    qDebug() << "Fetching" << numImages << "random images...";

    for (int i = 0; i < numImages; ++i) {
        try {
            // Validate URL
            QUrl url("https://10.101.3.119:50169/generate");
            if (!url.isValid()) {
                qDebug("Invalid API URL");
                continue;
            }

            QNetworkRequest request(url);

            // SSL Configuration with error handling
            try {
                QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
                sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
                request.setSslConfiguration(sslConfig);
            } catch (const std::exception& e) {
                qDebug("SSL Configuration error");
                continue;
            }

            // Validate API key
            QByteArray apiKey = getApiKey().toUtf8();

            if (apiKey.isEmpty()) {
                qDebug("Missing API key");
                continue;
            }
            request.setRawHeader("x-api-key", apiKey);

            // Validate and set query parameters
            QString prompt = getPrompt();
            if (prompt.isEmpty()) {
                qDebug("Empty prompt");
                continue;
            }

            double lowThreshold = getLowThreshold();
            double highThreshold = getHighThreshold();

            QUrlQuery query;
            query.addQueryItem("prompt", prompt);
            query.addQueryItem("style", getIsRealistic() ? "realistic" : "animated");
            query.addQueryItem("lo_threshold", QString::number(lowThreshold));
            query.addQueryItem("hi_threshold", QString::number(highThreshold));

            // Image processing with error handling
            cv::Mat frameImage = m_selectedFrame->getImage();
            if (frameImage.empty()) {
                qDebug("Failed to get frame image");
                continue;
            }

            std::vector<uchar> buf;
            try {
                if (!cv::imencode(".jpg", frameImage, buf)) {
                    qDebug("Failed to encode image");
                    continue;
                }
            } catch (const cv::Exception& e) {
                qDebug("OpenCV error");
                continue;
            }

            // Prepare POST data
            QByteArray postData;
            // Add your POST data here

            // Send request with timeout
            QNetworkReply *reply = m_networkManager->post(request, postData);
            if (!reply) {
                qDebug("Failed to create network request");
                continue;
            }

            // Set timeout for request
            QTimer *timer = new QTimer(this);
            timer->setSingleShot(true);
            connect(timer, &QTimer::timeout, this, [reply, timer]() {
                if (reply->isRunning()) {
                    reply->abort();
                    reply->deleteLater();
                    timer->deleteLater();
                }
            });
            timer->start(30000); // 30 second timeout

            // Handle response
            connect(reply, &QNetworkReply::finished, this, [this, reply, timer]() {
                timer->stop();
                timer->deleteLater();

                if (reply->error() == QNetworkReply::NoError) {
                    // Check response code
                    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                    if (httpStatus >= 200 && httpStatus < 300) {
                        handleImageResponse(reply);
                    } else {
                        qDebug("HTTP error:");
                    }
                } else {
                    // Handle specific network errors
                    switch (reply->error()) {
                    case QNetworkReply::ConnectionRefusedError:
                        qDebug("Connection refused");
                        break;
                    case QNetworkReply::TimeoutError:
                        qDebug("Request timed out");
                        break;
                    case QNetworkReply::SslHandshakeFailedError:
                        qDebug("SSL handshake failed");
                        break;
                    default:
                        qDebug("Network error:");
                    }
                }
                reply->deleteLater();
            });

            // Handle network errors during transmission
            connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError error) {
                qDebug("Network error during transmission");
            });

        } catch (const std::exception& e) {
            qDebug("Unexpected error");
            continue;
        }
    }
}

void PickImagesPage::handleImageResponse(QNetworkReply* reply)
{
    static int currentFrameIndex = 0;

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

                if (currentFrameIndex < m_imageFrames.size()) {
                    qDebug() << "Setting image for frame" << currentFrameIndex;
                    m_imageFrames[currentFrameIndex]->setImage(mat);
                    currentFrameIndex++;

                    // Reset counter if we've filled all frames
                    if (currentFrameIndex >= m_imageFrames.size()) {
                        currentFrameIndex = 0;
                    }
                }
            }
        }
    }
    reply->deleteLater();
}


void PickImagesPage::onRejectButtonClicked()
{
    refreshImages();
    QTimer::singleShot(300, this, [this]() {
        emit navigateToTextVisionPage();
    });
}

void PickImagesPage::onRetakePhotoButtonClicked()
{
    refreshImages();
    QTimer::singleShot(100, this, [this]() {
        emit navigateToSensitivityPage();
    });
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
    qDebug() << "Clearing images...";
    // Clear all frame images properly
    for (int i = 0; i < m_imageFrames.size(); ++i) {
        ClickableFrame* frame = m_imageFrames[i];
        // Explicitly clear image data
        cv::Mat emptyImage = cv::Mat::zeros(frame->size().height(), frame->size().width(), CV_8UC3);
        frame->setImage(emptyImage);
        qDebug() << "Cleared frame" << i;
    }
}

void PickImagesPage::refreshImages()
{
    qDebug() << "Starting refresh...";

    // Clear selection first
    if (m_selectedFrame) {
        m_selectedFrame->setSelected(false);
        m_selectedFrame = nullptr;
    }

    // Now fetch new images
    qDebug() << "Fetching new images...";
    fetchRandomImages(2);
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
        qDebug("is null");
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


