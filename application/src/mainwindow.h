#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

#include "pages/sidebar_pages/userpage.h"
#include "pages/sidebar_pages/favoritespage.h"
#include "pages/sidebar_pages/settingspage.h"

#include "pages/createpage.h"
#include "pages/takepicture.h"

#include "pages/acceptpicturepage.h"
#include "pages/sensitivitypage.h"
#include "pages/textvisionpage.h"
#include "pages/pickimagespage.h"
#include "pages/projectpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        // functions
        void loadStyleSheet();
        void setupUI();
        void setupPages();
        void setupConnections();
        QWidget* createSidebar();
        QPushButton* createSidebarButton(const QIcon& icon);


        Ui::MainWindow *ui;
        QStackedWidget *stackedWidget;

        // pages
        UserPage *userPage;
        FavoritesPage *favoritesPage;
        SettingsPage *settingsPage;

        CreatePage *createPage;
        TakePicture *takePicture;

        AcceptPicturePage *acceptPicturePage;
        SensitivityPage *sensitivityPage;
        TextVisionPage *textVisionPage;
        PickImagesPage *pickImagesPage;
        ProjectPage *projectPage;

        // sidebar buttons
        QPushButton *userButton;
        QPushButton *favoriteButton;
        QPushButton *createButton;
        QPushButton *settingsButton;

    private slots:
        // main pages
        void navigateToUserPage();
        void navigateToFavoritesPage();
        void navigateToSettingsPage();
        void navigateToCreatePage();

        // create page pages
        void navigateToPicturePage();
        void navigateToAcceptPicturePage();
        void navigateToSensitivityPage(const QImage &image);
        void navigateToTextVisionPage();
        void navigateToPickImagesPage();
        void navigateToProjectPage();

        // for the picture
        void setImageForAcceptPage(const QImage &image);

};
#endif // MAINWINDOW_H
