#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include <QWidget>

namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPage(QWidget *parent = nullptr);
    ~CalibrationPage();

private:
    Ui::CalibrationPage *ui;
};

#endif // CALIBRATIONPAGE_H
