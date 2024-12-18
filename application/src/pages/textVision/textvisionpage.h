#ifndef TEXTVISIONPAGE_H
#define TEXTVISIONPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>

namespace Ui {
class TextVisionPage;
}

class TextVisionPage : public QWidget
{
    Q_OBJECT

public:
    explicit TextVisionPage(QWidget *parent = nullptr);
    ~TextVisionPage();

    QString getVisionText() const { return m_visionText; }
    bool isRealistic() const { return m_isRealistic; }
    void clearInput();

    void hideLoading(); // hide loading

signals:
    void navigateToPickImagesPage(QString prompt, bool isRealistic);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSubmitButtonClicked();
    void onRealisticButtonClicked();
    void onAnimatedButtonClicked();
    void onTextChanged();

private:
    Ui::TextVisionPage *ui;

    void setupUI();
    void setupLayouts();
    void setupStyleSheets();
    void setupConnections();
    QLabel* createTitleLabel();

    void showKeyboard();
    void hideKeyboard();

    void updateButtonStyles();
    QPushButton* createSubmitButton();

    QString m_wvkbdPath; // keyboard path
    bool m_onRaspberryPi;
    QLabel *m_title;
    QPushButton *m_realisticButton;
    QPushButton *m_animatedButton;
    QTextEdit *m_visionInput;
    QPushButton *m_submitButton;
    bool isRunningOnRaspberryPi();


    // for loading feature
    void showLoading();



    QString m_visionText;
    bool m_isRealistic;

    const QString SELECTED_STYLE = "color: #FFD700; background-color: #4E4E4E; "
                                   "border: 2px solid #FFD700; border-radius: 15px;"
                                   " min-width: 120px; max-width: 120px;"
                                   " min-height: 30px; max-height: 30px;";
    const QString UNSELECTED_STYLE = "color: white; background-color: #3E3E3E; "
                                     "border: 2px solid #3E3E3E; border-radius: 15px;"
                                     " min-width: 120px; max-width: 120px;"
                                     " min-height: 35px; max-height: 35px;";
};

#endif // TEXTVISIONPAGE_H
