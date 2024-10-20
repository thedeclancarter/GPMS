#include "mainwindow.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QInputMethod>

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    // for rasp pi
    #ifdef Q_OS_RASPBERRYPI
        qputenv("QT_QPA_PLATFORM", QByteArray("eglfs"));
    #endif

    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(1024, 600);
    w.show();
    return a.exec();
}
