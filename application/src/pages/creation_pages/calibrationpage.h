#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include "imageprojectionwindow.h"
#include <QWidget>

namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

    public:
        explicit CalibrationPage(ImageProjectionWindow *projectionWindow, QWidget *parent = nullptr);
        ~CalibrationPage();

    private:
        Ui::CalibrationPage *ui;
        ImageProjectionWindow *m_projectionWindow;
        void onCalibrateButtonClicked();

    signals:
        void navigateToSensitivityPage();
};

#endif // CALIBRATIONPAGE_H
