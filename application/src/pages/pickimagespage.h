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
#include <clickableframe.h>

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
        void refreshImages();
        void resetState();

        // Getters
        QString getPrompt() const { return m_prompt; }
        bool getIsRealistic() const { return m_isRealistic; }
        double getLowThreshold() const { return m_lowThreshold; }
        double getHighThreshold() const { return m_highThreshold; }
        void fetchRandomImages(int numImages);

        // Setters
        void setPrompt(const QString& prompt) { m_prompt = prompt; }
        void setIsRealistic(bool isRealistic) { m_isRealistic = isRealistic; }
        void setLowThreshold(double threshold) { m_lowThreshold = threshold; }
        void setHighThreshold(double threshold) { m_highThreshold = threshold; }
        void setAPIImage(QImage image) { m_actual_image = image; }

    signals:
        void navigateToTextVisionPage(int low = 150, int high= 15);
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
        QImage m_actual_image;

        // for api
        QMap<QNetworkReply*, QTimer*> m_replyTimers;
        QList<QNetworkReply*> m_activeReplies;  // Track active network replies

        void cleanupNetworkRequests();  // New method for cleanup
        bool validateInputs(int numImages);
        QNetworkRequest createNetworkRequest();
        QUrlQuery createQueryParameters();
        cv::Mat prepareImageData();
        QByteArray encodeToPNG(const cv::Mat& image);
        void sendNetworkRequest(const QNetworkRequest& request, const QByteArray& postData);
        void setupRequestTimeout(QNetworkReply* reply);
        void setupResponseHandlers(QNetworkReply* reply);
        void handleNetworkReply(QNetworkReply* reply);
        QString getApiKey();


        void initializeUI();
        QLabel* createTitleLabel();
        QFrame* createImagesGrid();
        QHBoxLayout* createButtonLayout();
        QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
        void updateSelectedImages(ClickableFrame *clickedFrame);


        // image methods
        cv::Mat qimage_to_mat(const QImage& img);
        void clearImages();



};

#endif // PICKIMAGESPAGE_H
