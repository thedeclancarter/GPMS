#include "pickimagespage.h"
#include "ui_pickimagespage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

ClickableFrame::ClickableFrame(QWidget *parent) : QFrame(parent), m_selected(false)
{
    updateStyle();

    // Add a layout to the frame for displaying an image
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);  // Ensure layout is applied to frame
}

void ClickableFrame::setSelected(bool selected)
{
    qDebug("In setSelected(), selected: %d", selected);
    m_selected = selected;
    updateStyle();
}

bool ClickableFrame::isSelected() const
{
    return m_selected;
}

void ClickableFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug("Frame clicked, previous selection state: %d", m_selected);
        setSelected(!m_selected);
        emit clicked();
        qDebug("Clicked signal emitted, new selection state: %d", m_selected);
    }
    QFrame::mousePressEvent(event);
}

void ClickableFrame::updateStyle()
{
    // Define different styles for selected and unselected states
    QString baseStyle = R"(
        ClickableFrame {
            border-radius: 20px;
            background-color: #3E3E3E;
            border: %1px solid %2;
        })";

    QString selectedBorderColor = "#FFD700";  // Gold color for the selected frame
    QString unselectedBorderColor = "#3E3E3E";  // Default background color for unselected frames

    // Update border thickness and color based on selection state
    QString style = baseStyle.arg(m_selected ? "5" : "1", m_selected ? selectedBorderColor : unselectedBorderColor);
    setStyleSheet(style);

    // Add a shadow effect when selected
    if (m_selected) {
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(20);  // Shadow blur intensity
        shadowEffect->setColor(QColor(255, 215, 0, 160));  // Semi-transparent gold shadow
        shadowEffect->setOffset(0, 0);  // Position shadow directly under the frame
        setGraphicsEffect(shadowEffect);

        // Create a property animation to scale the frame when clicked (fun effect)
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(300);
        QRect startGeometry = geometry();
        QRect endGeometry = QRect(startGeometry.x() - 10, startGeometry.y() - 10,
                                  startGeometry.width() + 20, startGeometry.height() + 20);
        animation->setStartValue(startGeometry);
        animation->setEndValue(endGeometry);
        animation->setEasingCurve(QEasingCurve::OutBounce);  // Fun bouncing effect
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        // Remove shadow if the frame is unselected
        setGraphicsEffect(nullptr);
    }
}


void PickImagesPage::handleImageResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qDebug() << "No reply received";
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Image received successfully";
        QByteArray imageData = reply->readAll();

        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << "Content-Type: " << contentType;

        if (contentType.startsWith("image")) {
            QPixmap pixmap;
            if (pixmap.loadFromData(imageData)) {
                qDebug() << "Pixmap loaded from network reply";

                // Iterate through the frames and set pixmap for the first available frame
                for (int i = 0; i < m_imageFrames.size(); ++i) {
                    ClickableFrame* frame = m_imageFrames.at(i);

                    // Check if the frame has any children (e.g., QLabel for image)
                    if (frame->layout()->isEmpty()) {
                        QLabel* imageLabel = new QLabel(frame);

                        // Scale the pixmap to fit the frame
                        imageLabel->setPixmap(pixmap.scaled(frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

                        // Add the QLabel to the frame's layout
                        frame->layout()->addWidget(imageLabel);

                        // Ensure layout updates
                        frame->update();
                        frame->adjustSize();

                        break;  // Break after filling the first available frame
                    }
                }
            } else {
                qDebug() << "Failed to load pixmap from data";
            }
        } else {
            qDebug() << "Received content is not an image!";
        }
    } else {
        qDebug() << "Network reply error: " << reply->errorString();
    }

    reply->deleteLater();
}

void PickImagesPage::fetchRandomImages()
{
    qDebug() << "Fetching random images...";

    QNetworkRequest request1(QUrl("https://picsum.photos/200/150"));
    request1.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply1 = m_networkManager->get(request1);

    QNetworkRequest request2(QUrl("https://picsum.photos/200/150"));
    request2.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply2 = m_networkManager->get(request2);

    // Make sure to connect each reply to the image handler
    connect(reply1, &QNetworkReply::finished, this, &PickImagesPage::handleImageResponse);
    connect(reply2, &QNetworkReply::finished, this, &PickImagesPage::handleImageResponse);
}

PickImagesPage::PickImagesPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PickImagesPage)
    , m_selectedFrame(nullptr)
    , m_networkManager(new QNetworkAccessManager(this)) // Add network manager
{
    ui->setupUi(this);
    initializeUI();

    QCoreApplication::addLibraryPath("C:/Program Files/openssl-1.1/x64/bin");
    connect(ui->selectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onAcceptButtonClicked);
    connect(ui->rejectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onRejectButtonClicked);
    connect(ui->retakePhotoButton, &QPushButton::clicked, this, &PickImagesPage::onRetakePhotoButtonClicked);

    // Fetch random images when initializing the UI
    fetchRandomImages();
}

PickImagesPage::~PickImagesPage()
{
    delete ui;
}

void PickImagesPage::initializeUI()
{
    setStyleSheet("background-color: #1E1E1E;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(createTitleLabel(), 0, Qt::AlignHCenter);
    mainLayout->addWidget(createImagesGrid(), 1);
    mainLayout->addLayout(createButtonLayout());

    setLayout(mainLayout);
}

QLabel* PickImagesPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Pick The Images You like Best", this);
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
    titleLabel->setFixedWidth(800);
    return titleLabel;
}

QFrame* PickImagesPage::createImagesGrid()
{
    QFrame *gridFrame = new QFrame(this);
    gridFrame->setFrameStyle(QFrame::NoFrame);
    gridFrame->setStyleSheet("background-color: transparent;");

    QGridLayout *gridLayout = new QGridLayout(gridFrame);
    gridLayout->setSpacing(10);

    for (int i = 0; i < 2; ++i) {
        ClickableFrame *imageFrame = new ClickableFrame(this);
        imageFrame->setMinimumSize(200, 150);
        connect(imageFrame, &ClickableFrame::clicked, this, [this, imageFrame]() {
            qDebug("Calling update selected images");
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
    qDebug("In update selected images");
    if (m_selectedFrame && m_selectedFrame != clickedFrame) {
        qDebug("Deselecting previous frame");
        m_selectedFrame->setSelected(false);  // Deselect the previous frame
    }

    // clickedFrame->setSelected(!clickedFrame->isSelected());

    if (clickedFrame->isSelected()) {
        qDebug("Frame deselected");
        m_selectedFrame = clickedFrame;  // Update to the newly selected frame
    } else {
        qDebug("Frame deselected");
        m_selectedFrame = nullptr;  // Clear the selection if the frame was deselected
    }

    // Enable or disable the select button based on whether any frame is selected
    qDebug("Setting selectImagesButton enabled state to %d", m_selectedFrame != nullptr);
    ui->selectImagesButton->setEnabled(m_selectedFrame != nullptr);
}

QHBoxLayout* PickImagesPage::createButtonLayout()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(styleButton(ui->retakePhotoButton, "RETAKE PHOTO", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->rejectImagesButton, "REVISE MY VISION", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->selectImagesButton, "CHOOSE PICTURE", "#BB64c7"));
    ui->selectImagesButton->setEnabled(false);  // Initially disabled
    return buttonLayout;
}

QPushButton* PickImagesPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    // Define hover colors as 30% darker versions of the original colors
    QString hoverColor;
    if (bgColor == "#CD6F6F") {
        hoverColor = "#9F5D5D"; // 30% darker version of #CD6F6F
    } else if (bgColor == "#BB64c7") {
        hoverColor = "#83468B"; // 30% darker version of #6FCD6F
    } else {
        hoverColor = "#FFD700"; // Fallback hover color (for other cases)
    }

    button->setText(text);
    button->setFixedHeight(50);
    button->setFixedWidth(250);
    button->setStyleSheet(QString(
                              "QPushButton {"
                              "   background-color: %1;"   // Original color
                              "   color: black;"
                              "   border-radius: 25px;"
                              "   font-weight: bold;"
                              "   font-size: 16px;"
                              "   padding: 0 20px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: %2;"   // 30% darker color for hover
                              "}"
                              "QPushButton:pressed {"
                              "   background-color: darker(%1, 140%);" // Ensure the pressed color is darker
                              "}"
                              "QPushButton:disabled {"
                              "   background-color: #808080;"  // Disabled state color
                              "}"
                              ).arg(bgColor).arg(hoverColor));

    // Set the hand cursor when hovering over the button
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

void PickImagesPage::onRejectButtonClicked()
{
    clearImages();  // Clear the current images
    fetchRandomImages();  // Fetch new images
    emit navigateToTextVisionPage();
}

void PickImagesPage::onRetakePhotoButtonClicked()
{
    clearImages();  // Clear the current images
    fetchRandomImages();  // Fetch new images
    emit navigateToSensitivityPage();
}

void PickImagesPage::clearImages()
{
    // Iterate through the frames and remove all child widgets (i.e., the QLabel holding the images)
    for (int i = 0; i < m_imageFrames.size(); ++i) {
        QLayout* layout = m_imageFrames[i]->layout();
        while (QLayoutItem* item = layout->takeAt(0)) {
            delete item->widget();  // Delete the QLabel
        }
    }
}

void PickImagesPage::onAcceptButtonClicked()
{
    if (!m_selectedFrame) {
        qDebug("No frame selected");
        return;
    }

    int selectedIndex = m_imageFrames.indexOf(m_selectedFrame);
    qDebug("Selected frame index: %d", (selectedIndex));

    QList<int> selectedIndices;
    selectedIndices.append(selectedIndex);

    emit navigateToProjectPage(selectedIndices);
}
