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

        // Getters
        QString getPrompt() const { return m_prompt; }
        bool getIsRealistic() const { return m_isRealistic; }
        double getLowThreshold() const { return m_lowThreshold; }
        double getHighThreshold() const { return m_highThreshold; }

        // Setters
        void setPrompt(const QString& prompt) { m_prompt = prompt; }
        void setIsRealistic(bool isRealistic) { m_isRealistic = isRealistic; }
        void setLowThreshold(double threshold) { m_lowThreshold = threshold; }
        void setHighThreshold(double threshold) { m_highThreshold = threshold; }

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

        // to pass in for the api
        QString m_prompt;
        bool m_isRealistic;
        double m_lowThreshold;
        double m_highThreshold;

        QString getApiKey();


        void initializeUI();
        QLabel* createTitleLabel();
        QFrame* createImagesGrid();
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
        void updateSelectedImages(ClickableFrame *clickedFrame);
        void fetchRandomImages(int numImages);

        // image methods
        cv::Mat qimage_to_mat(const QImage& img);
        void clearImages();

};

#endif // PICKIMAGESPAGE_H
