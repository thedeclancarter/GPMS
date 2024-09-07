#ifndef IMAGEPROJECTIONWINDOW_H
#define IMAGEPROJECTIONWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>

class ImageProjectionWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImageProjectionWindow(QWidget *parent = nullptr);

private:
    QLabel *imageLabel;

    void setupUI();
};

#endif // IMAGEPROJECTIONWINDOW_H
