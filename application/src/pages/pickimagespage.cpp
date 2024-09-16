#include "pickimagespage.h"
#include "ui_pickimagespage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QMouseEvent>

ClickableFrame::ClickableFrame(QWidget *parent) : QFrame(parent), m_selected(false)
{
    updateStyle();

    // Newly added 9/14
    // Add a layout to the frame for displaying an image
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
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
    QString style = QString(
                        "ClickableFrame {"
                        "   border-radius: 20px;"
                        "   background-color: #3E3E3E;"
                        "   border: %1px solid %2;"
                        "}"
                        ).arg(m_selected ? "4" : "2", m_selected ? "#FFD700" : "#3E3E3E");

    qDebug("Applying stylesheet: %s", qPrintable(style));
    setStyleSheet(style);
}

PickImagesPage::PickImagesPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PickImagesPage)
    ,m_selectedFrame(nullptr)  // Initialize to nullptr
{
    ui->setupUi(this);
    initializeUI();

    connect(ui->selectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onAcceptButtonClicked);
    connect(ui->rejectImagesButton, &QPushButton::clicked, this, &PickImagesPage::onRejectButtonClicked);
    connect(ui->retakePhotoButton, &QPushButton::clicked, this, &PickImagesPage::onRetakePhotoButtonClicked);
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

    mainLayout->addWidget(createTitleLabel());
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
    buttonLayout->addWidget(styleButton(ui->rejectImagesButton, "REVISE MY VISION", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->retakePhotoButton, "RETAKE PHOTO", "#CD6F6F"));
    buttonLayout->addWidget(styleButton(ui->selectImagesButton, "CHOOSE PICTURE", "#6FCD6F"));
    ui->selectImagesButton->setEnabled(false);  // Initially disabled
    return buttonLayout;
}

QPushButton* PickImagesPage::styleButton(QPushButton* button, const QString& text, const QString& bgColor)
{
    button->setText(text);
    button->setFixedHeight(50);
    button->setStyleSheet(QString(
                              "QPushButton {"
                              "   background-color: %1;"
                              "   color: white;"
                              "   border-radius: 25px;"
                              "   font-weight: bold;"
                              "   font-size: 16px;"
                              "   padding: 0 20px;"
                              "}"
                              "QPushButton:hover {"
                              "   background-color: #FFD700;"
                              "}"
                              "QPushButton:pressed {"
                              "   background-color: darker(%1, 140%);"
                              "}"
                              "QPushButton:disabled {"
                              "   background-color: #808080;"
                              "}"
                              ).arg(bgColor));
    return button;
}

void PickImagesPage::onRejectButtonClicked()
{
    emit navigateToTextVisionPage();
}

void PickImagesPage::onRetakePhotoButtonClicked()
{
    emit navigateToSensitivityPage();
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
