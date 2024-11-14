// clickableframe.cpp

#include "clickableframe.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QMouseEvent>

ClickableFrame::ClickableFrame(QWidget *parent)
    : QFrame(parent),
      m_selected(false),
      m_imageLabel(nullptr),
      m_loadingLabel(nullptr),
      m_image(cv::Mat()),
      m_loadingTimer(new QTimer(this)),
      m_loadingBaseText("Loading Your Image"),
      m_dotCount(0)
{
    setLayout(new QVBoxLayout(this));

    // Create loading label without dots
    m_loadingLabel = new QLabel(m_loadingBaseText, this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("QLabel { color: #FFFFFF; font-size: 16px; }");

    // Create image label
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setContentsMargins(5, 5, 5, 5);

    // Stack the labels
    m_imageLabel->setVisible(false);  // Initially hide the image label
    layout()->addWidget(m_loadingLabel);
    layout()->addWidget(m_imageLabel);

    updateStyle();

    // Setup loading timer
    connect(m_loadingTimer, &QTimer::timeout, this, &ClickableFrame::updateLoadingDots);
}

void ClickableFrame::setImage(const cv::Mat& mat)
{
    // Stop the loading animation as the image is received
    m_loadingTimer->stop();
    m_loadingLabel->setVisible(false);

    if (mat.empty()) {
        m_imageLabel->setVisible(false);
        m_loadingLabel->setVisible(true);
        m_loadingTimer->start(500); // Restart loading animation if needed
        m_image = cv::Mat();
        return;
    }

    if (mat.type() != CV_8UC3) {
        qDebug() << "Invalid image format. Expected CV_8UC3.";
        return;
    }

    m_image = mat.clone();

    // Convert cv::Mat to QImage
    QImage qimg(m_image.data, m_image.cols, m_image.rows, m_image.step, QImage::Format_RGB888);
    if (qimg.isNull()) {
        qDebug() << "Failed to create QImage";
        clearImage();
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(qimg);
    m_imageLabel->setPixmap(pixmap);  // Display the image without scaling

    m_imageLabel->setVisible(true);
    m_loadingLabel->setVisible(false);
}

void ClickableFrame::clearImage()
{
    m_image = cv::Mat();
    m_imageLabel->clear();
    m_imageLabel->setVisible(false);
    m_loadingLabel->setVisible(true);
    m_loadingTimer->start(500); // Start loading animation
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

bool ClickableFrame::hasValidImage() const {
    return !m_image.empty();
}

cv::Mat ClickableFrame::getImage() const
{
    return hasValidImage() ? m_image : cv::Mat();
}

void ClickableFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && hasValidImage())
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

void ClickableFrame::updateLoadingDots()
{
    m_dotCount = (m_dotCount + 1) % 4; // Cycle through 0 to 3 dots
    QString dots = QString(m_dotCount, '.');
    m_loadingLabel->setText(m_loadingBaseText + dots);
}
