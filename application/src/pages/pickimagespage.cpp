#include "pickimagespage.h"
#include "ui_pickimagespage.h"
#include "../utils/image_utils.h"
#include "imageprojectionwindow.h"
#include "clickableframe.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QTimer>
#include <QDir>
#include <QUrlQuery>
#include <QProcessEnvironment>
#include <QPainter>


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
    , m_actual_image(0,0)
{
    ui->setupUi(this);
    initializeUI();

    QCoreApplication::addLibraryPath("C:/Program Files/openssl-1.1/x64/bin");
    // connect buttons
    connect(ui->selectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onAcceptButtonClicked);
    connect(ui->rejectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onRejectButtonClicked);
    connect(ui->retakePhotoButton, &QPushButton::clicked, this, &PickImagesPage::onRetakePhotoButtonClicked);
}

PickImagesPage::~PickImagesPage()
{
    cleanupNetworkRequests();
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
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("API_KEY", "");

    if (apiKey.isEmpty()) {
        qDebug() << "Warning: MY_API_KEY environment variable not found";
    }

    return apiKey;
}

void PickImagesPage::fetchRandomImages(int numImages) {
    if (!validateInputs(numImages)) {
        return;
    }

    for (int i = 0; i < numImages; ++i) {
        try {
            qDebug("Creating network request");
            QNetworkRequest request = createNetworkRequest();
            qDebug("Created network request");
            cv::Mat imageData = prepareImageData();

            if (imageData.empty()) {
                qDebug() << "Failed to prepare image data";
                continue;
            }

            QByteArray postData = encodeToPNG(imageData);
            if (postData.isEmpty()) {
                qDebug() << "Failed to encode image to PNG";
                continue;
            }
            qDebug() << "sending network request: " << i;
            sendNetworkRequest(request, postData);

        } catch (const std::exception& e) {
            qDebug() << "Unexpected error:" << e.what();
            continue;
        }
    }
}

bool PickImagesPage::validateInputs(int numImages) {
    if (numImages <= 0 || m_actual_image.isNull()) {
        qDebug() << "Invalid number of images or empty actual image";
        return false;
    }
    return true;
}


QUrl getEnvironmentUrl() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString host = env.value("API_HOST", "");
    QString port = env.value("API_PORT", "");
    QString endpoint = env.value("API_ENDPOINT", "");

    // Construct the URL
    QString urlStr = QString("https://%1:%2/%3")
                         .arg(host)
                         .arg(port)
                         .arg(endpoint);

    qDebug() << "Generated URL:" << urlStr;

    return QUrl(urlStr);
}

QNetworkRequest PickImagesPage::createNetworkRequest() {


    QUrl url = getEnvironmentUrl();

    if (!url.isValid()) {
        throw std::runtime_error("Invalid API URL");
    }

    QNetworkRequest request(url);

    // SSL Configuration
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    // Set API Key
    QByteArray apiKey = getApiKey().toUtf8();
    if (apiKey.isEmpty()) {
        throw std::runtime_error("Missing API key");
    }
    request.setRawHeader("x-api-key", apiKey);

    // Instead of setting query parameters, we'll send them in the multipart data
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=boundary");

    return request;
}

QUrlQuery PickImagesPage::createQueryParameters() {
    QString prompt = getPrompt();
    if (prompt.isEmpty()) {
        throw std::runtime_error("Empty prompt");
    }

    QUrlQuery query;
    query.addQueryItem("prompt", prompt);
    query.addQueryItem("style", getIsRealistic() ? "realistic" : "animated");
    query.addQueryItem("lo_threshold", QString::number(getLowThreshold()));
    query.addQueryItem("hi_threshold", QString::number(getHighThreshold()));

    return query;
}

cv::Mat PickImagesPage::prepareImageData() {
    qDebug() << "Original pixmap size:" << m_actual_image.size();
    // Convert to QImage
    QImage qImage = m_actual_image;

    // Scale to exactly 1280x720 while maintaining aspect ratio
    qImage = qImage.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // If the image isn't exactly 1280x720 after scaling (due to aspect ratio),
    // create a black background image and center the scaled image
    if (qImage.size() != QSize(1280, 720)) {
        QImage finalImage(1280, 720, QImage::Format_RGB888);
        finalImage.fill(Qt::black);

        // Calculate position to center the image
        int x = (1280 - qImage.width()) / 2;
        int y = (720 - qImage.height()) / 2;

        // Create painter to draw the scaled image onto the black background
        QPainter painter(&finalImage);
        painter.drawImage(x, y, qImage);
        painter.end();

        qImage = finalImage;
    }

    cv::Mat mat = ImageUtils::qimage_to_mat(qImage);
    qDebug() << "Resulting Mat size:" << mat.size().width << "x" << mat.size().height;

    return mat;
}

QByteArray PickImagesPage::encodeToPNG(const cv::Mat& image) {
    std::vector<uchar> buffer;
    std::vector<int> png_params;
    png_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    png_params.push_back(9); // Maximum compression

    if (!cv::imencode(".png", image, buffer, png_params)) {
        return QByteArray();
    }

    return QByteArray(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

void PickImagesPage::cleanupNetworkRequests()
{
    // Abort and cleanup all active network requests
    for (QNetworkReply* reply : m_activeReplies) {
        if (reply) {
            disconnect(reply, nullptr, this, nullptr);  // Disconnect all signals
            reply->abort();  // Abort the request
            reply->deleteLater();
            qDebug() << "Ending network request for: " << m_prompt;
        }
    }
    m_activeReplies.clear();

    // Clean up associated timers
    for (QTimer* timer : m_replyTimers.values()) {
        if (timer) {
            timer->stop();
            timer->deleteLater();
        }
    }
    m_replyTimers.clear();
}

void PickImagesPage::sendNetworkRequest(const QNetworkRequest& request, const QByteArray& imageData) {
    // Create multipart data
    QByteArray multipartData;
    QString boundary = "boundary";

    // Add form fields
    QMap<QString, QString> formFields;
    formFields["prompt"] = getPrompt();
    formFields["style"] = getIsRealistic() ? "realistic" : "animated";
    formFields["lo_threshold"] = QString::number(getLowThreshold());
    formFields["hi_threshold"] = QString::number(getHighThreshold());

    // Add form fields to multipart data
    for (auto it = formFields.begin(); it != formFields.end(); ++it) {
        multipartData.append("--" + boundary.toUtf8() + "\r\n");
        multipartData.append("Content-Disposition: form-data; name=\"" + it.key().toUtf8() + "\"\r\n\r\n");
        multipartData.append(it.value().toUtf8() + "\r\n");
    }

    // Add image data
    multipartData.append("--" + boundary.toUtf8() + "\r\n");
    multipartData.append("Content-Disposition: form-data; name=\"image\"; filename=\"image.png\"\r\n");
    multipartData.append("Content-Type: image/png\r\n\r\n");
    multipartData.append(imageData);
    multipartData.append("\r\n");

    // Add closing boundary
    multipartData.append("--" + boundary.toUtf8() + "--\r\n");

    // Send the request
    qDebug() << "posting request now";
    try {
        QNetworkReply *reply = m_networkManager->post(request, multipartData);
        if (!reply) {
            qDebug() << "Failed to create network reply";
            return;
        }

        m_activeReplies.append(reply);  // Track the new reply

        setupRequestTimeout(reply);
        setupResponseHandlers(reply);

    } catch (const std::exception& e) {
        qDebug() << "Exception in sendNetworkRequest:" << e.what();
    }
}

void PickImagesPage::setupRequestTimeout(QNetworkReply* reply) {
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);

    // Store the timer with its associated reply
    m_replyTimers[reply] = timer;

    // Explicitly capture timer in the lambda
    connect(timer, &QTimer::timeout, this, [this, reply, timer]() {
        if (m_replyTimers.contains(reply)) {
            // Check if the reply is still valid and running
            if (!reply->isFinished()) {
                reply->abort();
            }
            // Clean up
            m_replyTimers.remove(reply);
            timer->deleteLater();
            reply->deleteLater();
        }
    });

    // Explicitly capture timer in this lambda too
    connect(reply, &QNetworkReply::finished, this, [this, reply, timer]() {
        if (m_replyTimers.contains(reply)) {
            m_replyTimers.remove(reply);
            timer->stop();
            timer->deleteLater();
        }
    });

    timer->start(100000); // 100 second timeout
}

void PickImagesPage::setupResponseHandlers(QNetworkReply* reply) {
    // Handle completion
    qDebug() << "Got reply " << reply;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });

    // Handle errors during transmission
    connect(reply, &QNetworkReply::errorOccurred, this,
            [](QNetworkReply::NetworkError error) {
                qDebug() << "Network error during transmission:" << error;
            });
}


// connected from setup response handlers
void PickImagesPage::handleNetworkReply(QNetworkReply* reply) {
    // Ensure the reply is still valid
    if (!reply) {
        return;
    }

    // Remove from active replies list
    m_activeReplies.removeOne(reply);

    // Clean up the timer if it exists
    if (m_replyTimers.contains(reply)) {
        QTimer* timer = m_replyTimers[reply];
        m_replyTimers.remove(reply);
        timer->stop();
        timer->deleteLater();
    }

    if (reply->error() == QNetworkReply::NoError) {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (httpStatus >= 200 && httpStatus < 300) {
            handleImageResponse(reply);
        } else {
            qDebug() << "HTTP error:" << httpStatus;
        }
    } else {
        qDebug() << "Network error:" << reply->errorString();
    }

    reply->deleteLater();
}


// called by handle network reply
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
                        qDebug("Filled all frames, resetting index");
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
    cleanupNetworkRequests();
    QTimer::singleShot(300, this, [this]() {
        emit navigateToTextVisionPage();
    });
}

void PickImagesPage::onRetakePhotoButtonClicked()
{
    cleanupNetworkRequests();
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
        // frame->setImage(emptyImage);
        frame->clearImage();
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
    clearImages();
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
    gridFrame->setFixedWidth(1000);

    QGridLayout *gridLayout = new QGridLayout(gridFrame);
    gridLayout->setSpacing(10);

    for (int i = 0; i < 2; ++i)
    {
        ClickableFrame *imageFrame = new ClickableFrame(this);
        imageFrame->setFixedSize(450, 330);
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
