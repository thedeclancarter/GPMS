#include "textvisionpage.h"
#include "ui_textvisionpage.h"

#include <QVBoxLayout>
#include <QMessageBox>  // Include this header for QMessageBox on onSubmitButtonClicked function
#include <QFile>
#include <QDebug>


TextVisionPage::TextVisionPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextVisionPage)
    , m_isRealistic(true)
{

    // Initialize keyboard before setting up UI
    if (isRunningOnRaspberryPi()) {
        qDebug() << "Setting up virtual keyboard for Raspberry Pi";
        setupVirtualKeyboard();
    }

    setupUI();
    setupLayouts();
    setupStyleSheets();
    setupConnections();

    qDebug() << "Initializing TextVisionPage";

    if (isRunningOnRaspberryPi()) {
        m_visionInput->setAttribute(Qt::WA_InputMethodEnabled, true);
        m_visionInput->installEventFilter(this);
    }
}

bool TextVisionPage::isRunningOnRaspberryPi()
{
    // Method 1: Check model name in /proc/cpuinfo
    QFile cpuinfo("/proc/cpuinfo");
    if (cpuinfo.open(QFile::ReadOnly)) {
        QString content = cpuinfo.readAll();
        cpuinfo.close();

        if (content.contains("Raspberry Pi", Qt::CaseInsensitive) ||
            content.contains("BCM2", Qt::CaseInsensitive)) {
            qDebug("Detected Raspberry Pi via /proc/cpuinfo");
            return true;
        }
    }
    qDebug() << "Not running on Raspberry Pi";
    return false;
}

void TextVisionPage::clearInput(){
    m_visionInput->clear();
}

bool TextVisionPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_visionInput) {
        switch (event->type()) {
        case QEvent::FocusIn: {
            qDebug() << "Focus in event - showing keyboard";
            QInputMethod *inputMethod = QGuiApplication::inputMethod();
            if (inputMethod) {
                inputMethod->show();
                qDebug() << "Requested keyboard show";
            }
            return false;
        }
        case QEvent::FocusOut: {
            qDebug() << "Focus out event - hiding keyboard";
            QInputMethod *inputMethod = QGuiApplication::inputMethod();
            if (inputMethod) {
                inputMethod->hide();
                qDebug() << "Requested keyboard hide";
            }
            return false;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TextVisionPage::showVirtualKeyboard()
{
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (!inputMethod->isVisible()) {
        inputMethod->show();
    }
}

void TextVisionPage::hideVirtualKeyboard()
{
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (inputMethod->isVisible()) {
        inputMethod->hide();
    }
}

void TextVisionPage::toggleVirtualKeyboard()
{
    QInputMethod *inputMethod = QApplication::inputMethod();
    if (inputMethod->isVisible()) {
        hideVirtualKeyboard();
    } else {
        showVirtualKeyboard();
    }
}

void TextVisionPage::setupVirtualKeyboard()
{
    qDebug() << "Setting up virtual keyboard...";

    // Force the input method to be the virtual keyboard
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    // Check if input method is available
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (inputMethod) {
        qDebug() << "Input method is available";
        qDebug() << "Input method type:" << qgetenv("QT_IM_MODULE");

        // Connect to visibility changes
        connect(inputMethod, &QInputMethod::visibleChanged, this, [this]() {
            qDebug() << "Keyboard visibility changed:" << QGuiApplication::inputMethod()->isVisible();
            adjustLayoutForKeyboard();
        });
    } else {
        qDebug() << "Input method is NOT available";
    }
}

void TextVisionPage::adjustLayoutForKeyboard()
{
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (!inputMethod)
        return;

    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return;

    // Get keyboard rectangle in global coordinates
    QRectF keyboardRect = inputMethod->keyboardRectangle();
    qDebug() << "Keyboard rectangle:" << keyboardRect;

    if (inputMethod->isVisible()) {
        // Add bottom margin to avoid keyboard overlap
        setContentsMargins(0, 0, 0, keyboardRect.height());
    } else {
        // Reset margins when keyboard is hidden
        setContentsMargins(0, 0, 0, 0);
    }
}

void TextVisionPage::setupUI()
{
    m_title = createTitleLabel();

    m_realisticButton = new QPushButton("Realistic", this);
    m_animatedButton = new QPushButton("Animated", this);
    updateButtonStyles();

    m_visionInput = new QTextEdit(this);
    m_visionInput->setFixedSize(600, 200);
    m_visionInput->setAttribute(Qt::WA_InputMethodEnabled, true);

    m_submitButton = createSubmitButton();
    m_submitButton->setEnabled(false); // Initially disabled
    // Set the hand cursor when hovering over the button
    m_realisticButton->setCursor(Qt::PointingHandCursor);
    m_animatedButton->setCursor(Qt::PointingHandCursor);
}

QLabel* TextVisionPage::createTitleLabel()
{
    QLabel *titleLabel = new QLabel("Describe Your Vision In 1-2 Sentences!", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "background-color: #3E3E3E;"  // Same as webcam frame background
        "border-radius: 20px;"
        "padding: 15px 20px;"  // Vertical and horizontal padding
        );
    titleLabel->setFixedSize(800,60);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    return titleLabel;
}

void TextVisionPage::setupLayouts()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *optionsLayout = new QHBoxLayout();

    optionsLayout->addWidget(m_realisticButton);
    optionsLayout->addWidget(m_animatedButton);

    mainLayout->setSpacing(15);

    mainLayout->addWidget(m_title, 0, Qt::AlignHCenter);
    mainLayout->addLayout(optionsLayout);
    mainLayout->addWidget(m_visionInput, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_submitButton, 0, Qt::AlignHCenter);

    setLayout(mainLayout);
}

void TextVisionPage::setupStyleSheets()
{
    m_visionInput->setStyleSheet("color: white; background-color: #2E2E2E; border-radius: 10px; padding: 20px");
}

QPushButton* TextVisionPage::createSubmitButton()
{
    QPushButton* submit_button = new QPushButton("+ SUBMIT", this);
    submit_button->setFixedSize(120, 40);
    submit_button->setStyleSheet(
        "QPushButton {"
        "   background-color: #6F81CD;"
        "   color: white;"
        "   border-radius: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4E5A90;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5D5D5D;"
        "}"
        );

    // Set the hand cursor when hovering over the button
    submit_button->setCursor(Qt::PointingHandCursor);
    return submit_button;
}

void TextVisionPage::setupConnections()
{
    connect(m_submitButton, &QPushButton::clicked, this, &TextVisionPage::onSubmitButtonClicked);
    connect(m_realisticButton, &QPushButton::clicked, this, &TextVisionPage::onRealisticButtonClicked);
    connect(m_animatedButton, &QPushButton::clicked, this, &TextVisionPage::onAnimatedButtonClicked);
    connect(m_visionInput, &QTextEdit::textChanged, this, &TextVisionPage::onTextChanged);
}

TextVisionPage::~TextVisionPage()
{
    delete ui;
}

void TextVisionPage::updateButtonStyles()
{
    m_realisticButton->setStyleSheet(m_isRealistic ? SELECTED_STYLE : UNSELECTED_STYLE);
    m_animatedButton->setStyleSheet(m_isRealistic ? UNSELECTED_STYLE : SELECTED_STYLE);
}

// void TextVisionPage::onSubmitButtonClicked()
// {
//     m_visionText = m_visionInput->toPlainText();
//     emit navigateToPickImagesPage();
// }

void TextVisionPage::onSubmitButtonClicked()
{
    m_visionText = m_visionInput->toPlainText();

    if (m_visionText.isEmpty()) {
        // Optionally, show a message box to inform the user
        QMessageBox::warning(this, "Input Required", "Please enter text before submitting.");
        return;
    }

    emit navigateToPickImagesPage();
}


void TextVisionPage::onRealisticButtonClicked()
{
    m_isRealistic = true;
    updateButtonStyles();
}

void TextVisionPage::onAnimatedButtonClicked()
{
    m_isRealistic = false;
    updateButtonStyles();
}

void TextVisionPage::onTextChanged()
{
    bool hasText = !m_visionInput->toPlainText().isEmpty();
    m_submitButton->setEnabled(hasText);
}

