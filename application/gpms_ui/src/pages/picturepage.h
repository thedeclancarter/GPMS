#ifndef PICTUREPAGE_H
#define PICTUREPAGE_H

#include <QWidget>

namespace Ui {
class PicturePage;
}

class PicturePage : public QWidget
{
    Q_OBJECT

public:
    explicit PicturePage(QWidget *parent = nullptr);
    ~PicturePage();

signals:
    void navigateToAcceptPicturePage();

private slots:
    void onSubmitPictureButtonClicked();

private:
    Ui::PicturePage *ui;
};

#endif // PICTUREPAGE_H
