#ifndef IMAGEPROJECTIONWINDOW_H
#define IMAGEPROJECTIONWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>

class ImageProjectionWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImageProjectionWindow(const QImage &image, QWidget *parent = nullptr);

private:
    QLabel *imageLabel;

    void setupUI(const QImage &image);
};

#endif // IMAGEPROJECTIONWINDOW_H
