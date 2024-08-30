#include "favoritespage.h"
#include "ui_favoritespage.h"

FavoritesPage::FavoritesPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FavoritesPage)
{
    ui->setupUi(this);
}

FavoritesPage::~FavoritesPage()
{
    delete ui;
}
