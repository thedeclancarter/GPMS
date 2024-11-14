#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>
#include <QLabel>
#include <opencv2/core.hpp>

class ClickableFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ClickableFrame(QWidget *parent = nullptr);
    void setSelected(bool selected);
    bool isSelected() const;
    void setImage(const cv::Mat& mat);
    cv::Mat getImage() const;
    void clearImage();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_selected;
    cv::Mat m_image;
    QLabel* m_imageLabel;
    QLabel* m_loadingLabel;

    void updateStyle();
    bool hasValidImage() const;
};

#endif // CLICKABLEFRAME_H
