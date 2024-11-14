// clickableframe.h

#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>
#include <QLabel>
#include <QTimer>
#include <opencv2/opencv.hpp>

class ClickableFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ClickableFrame(QWidget *parent = nullptr);
    void setImage(const cv::Mat& mat);
    void clearImage();
    void setSelected(bool selected);
    bool isSelected() const;
    bool hasValidImage() const;
    cv::Mat getImage() const;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateLoadingDots();

private:
    void updateStyle();

    bool m_selected;
    QLabel* m_imageLabel;
    QLabel* m_loadingLabel;
    cv::Mat m_image;

    // New members for loading animation
    QTimer* m_loadingTimer;
    QString m_loadingBaseText;
    int m_dotCount;
};

#endif // CLICKABLEFRAME_H
