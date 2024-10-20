#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H
#include "imageprojectionwindow.h"

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <opencv2/imgproc.hpp>

namespace Ui {
class ProjectPage;
}

class ProjectPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectPage(ImageProjectionWindow *projectionWindow, QWidget *parent = nullptr);
    ~ProjectPage();
    void setSelectedImage(const cv::Mat& selectedImage);

signals:
    void navigateToPickImagesPage();
    void navigateToCreatePage();

private slots:
    void onRejectButtonClicked();
    void onDoneButtonClicked();

private:
    Ui::ProjectPage *ui;
    QLabel* m_imageLabel;

    // UI methods
    void initializeUI();
    QLabel* createTitleLabel();
    QFrame* createImageFrame();
    QHBoxLayout* createButtonLayout();
    QPushButton* styleButton(QPushButton* button, const QString& text, const QString& bgColor);
};

#endif // PROJECTPAGE_H
