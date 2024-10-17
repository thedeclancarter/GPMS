#ifndef PICKIMAGESPAGE_H
#define PICKIMAGESPAGE_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class PickImagesPage;
}

class ClickableFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ClickableFrame(QWidget *parent = nullptr);
    void setSelected(bool selected);
    bool isSelected() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

private:
    bool m_selected;
    void updateStyle();
};


class PickImagesPage : public QWidget
{
    Q_OBJECT

public:
    explicit PickImagesPage(QWidget *parent = nullptr);
    ~PickImagesPage();

signals:
    void navigateToTextVisionPage();
    void navigateToSensitivityPage();
    void navigateToProjectPage(const QList<int>& selectedIndices);

private slots:
    void onAcceptButtonClicked();
    void onRejectButtonClicked();
    void onRetakePhotoButtonClicked();

private:
    Ui::PickImagesPage *ui;
    QList<ClickableFrame*> m_imageFrames;
    ClickableFrame* m_selectedFrame;  // Member variable to store the selected frame
    QNetworkAccessManager *m_networkManager;  // Add this for managing network requests

    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createImagesGrid();
    QHBoxLayout* createButtonLayout();
    QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
    void updateSelectedImages(ClickableFrame *clickedFrame);

    // Methods for fetching and handling random images
    void fetchRandomImages();  // Fetch images from a remote source
    void handleImageResponse(void);  // Handle the response from the image fetch

    // New method for clearing the images before fetching new ones
    void clearImages();  // Clears the current images in the frames
};

#endif // PICKIMAGESPAGE_H
