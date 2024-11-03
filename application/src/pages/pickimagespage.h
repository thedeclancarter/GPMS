#ifndef PICKIMAGESPAGE_H
#define PICKIMAGESPAGE_H
#include "imageprojectionwindow.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


class ClickableFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ClickableFrame(QWidget *parent = nullptr);
    void setSelected(bool selected);
    bool isSelected() const;
    void setImage(const cv::Mat& mat);
    cv::Mat getImage() const;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_selected;
    cv::Mat m_image;
    QLabel* m_imageLabel;
    void updateStyle();
};

namespace Ui {
class PickImagesPage;
}


class PickImagesPage : public QWidget
{
    Q_OBJECT

public:
    explicit PickImagesPage(ImageProjectionWindow *projectionWindow, QWidget *parent = nullptr);
    ~PickImagesPage();
    cv::Mat getSelectedImage() const;
    void clearSelections();
    void refreshImages();

signals:
    void navigateToTextVisionPage();
    void navigateToSensitivityPage();
    void navigateToProjectPage(const cv::Mat& image);

private slots:
    void onAcceptButtonClicked();
    void onRejectButtonClicked();
    void onRetakePhotoButtonClicked();
    void handleImageResponse(QNetworkReply* reply);

private:
    Ui::PickImagesPage *ui;
    ImageProjectionWindow *m_projectionWindow;
    QList<ClickableFrame*> m_imageFrames;
    ClickableFrame* m_selectedFrame;
    QNetworkAccessManager *m_networkManager;

    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createImagesGrid();
    QHBoxLayout* createButtonLayout();
    QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
    void updateSelectedImages(ClickableFrame *clickedFrame);
    void fetchRandomImages(int numImages);

    // image methods
    cv::Mat qimage_to_mat(const QImage& img);
    void handleImageResponse();
    void fetchRandomImages();
    void clearImages();

};

#endif // PICKIMAGESPAGE_H
