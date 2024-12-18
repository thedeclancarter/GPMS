#include "textvisionpage.h"
#include "ui_textvisionpage.h"

#include <QVBoxLayout>
#include <QMessageBox>  // Include this header for QMessageBox on onSubmitButtonClicked function
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QEvent>
#include <QMouseEvent>

TextVisionPage::TextVisionPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextVisionPage)
    , m_isRealistic(true)
    , m_onRaspberryPi(isRunningOnRaspberryPi())

{

    setupUI();
    setupLayouts();
    setupStyleSheets();
    setupConnections();

    qDebug() << "Initializing TextVisionPage";


    if (m_onRaspberryPi) {
        qDebug() << "Setting up keyboard handling for Raspberry Pi";
        // Set the path to wvkbd in home directory
        m_wvkbdPath = QDir::homePath() + "/wvkbd";
        qDebug() << "wvkbd path set to:" << m_wvkbdPath;

        // Verify the path exists
        if (!QFile::exists(m_wvkbdPath)) {
            qDebug() << "Warning: wvkbd not found at" << m_wvkbdPath;
        }
        this->installEventFilter(this); // to see when clicking out
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
    if (m_onRaspberryPi) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (obj == m_visionInput) {
                qDebug() << "Click inside input - showing wvkbd";
                showKeyboard();
                m_visionInput->setFocus();
                return false;
            } else {
                qDebug() << "Click outside input - hiding wvkbd";
                hideKeyboard();
                m_visionInput->clearFocus();
                return false;
            }
            break;
        }
        case QEvent::FocusIn: {
            if (obj == m_visionInput) {
                qDebug() << "Focus in event - showing wvkbd";
                showKeyboard();
                return false;
            }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}



void TextVisionPage::showKeyboard()
{
    if (m_onRaspberryPi) {
        qDebug() << "Attempting to show keyboard from directory:" << m_wvkbdPath;

        qint64 pid;
        QString cmd = m_wvkbdPath + "/wvkbd-mobintl";

        // Using non-deprecated version of startDetached
        bool success = QProcess::startDetached(cmd, QStringList(), m_wvkbdPath, &pid);

        if (!success) {
            qDebug() << "Failed to start keyboard using primary method. Trying alternative...";

            // Fallback method using QProcess with non-deprecated start
            QProcess *process = new QProcess(this);
            process->setWorkingDirectory(m_wvkbdPath);
            process->start("./wvkbd-mobintl", QStringList(), QIODevice::ReadWrite);

            if (!process->waitForStarted()) {
                qDebug() << "Failed to start keyboard with fallback. Error:" << process->errorString();
                delete process;
            } else {
                qDebug() << "Keyboard started successfully with fallback method";
            }
        } else {
            qDebug() << "Keyboard started successfully with PID:" << pid;
        }
    }
}

void TextVisionPage::hideKeyboard()
{
    if (m_onRaspberryPi) {
        QProcess pkill;
        pkill.start("pkill", QStringList() << "-f" << "wvkbd-mobintl");
        pkill.waitForFinished();
        qDebug() << "Attempted to hide keyboard";
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

void TextVisionPage::showLoading()
{
    // m_submitButton->setText("Loading...");
    m_submitButton->setEnabled(false);
    m_visionInput->setEnabled(false);
}

void TextVisionPage::hideLoading()
{
    // m_submitButton->setText("+ SUBMIT");
    m_submitButton->setEnabled(true);
    m_visionInput->setEnabled(true);
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

void TextVisionPage::onSubmitButtonClicked()
{
    showLoading();
    m_visionText = m_visionInput->toPlainText();

    if (m_visionText.isEmpty()) {
        return;
    }

    emit navigateToPickImagesPage(m_visionText, m_isRealistic);
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

