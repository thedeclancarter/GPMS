#ifndef FAVORITESPAGE_H
#define FAVORITESPAGE_H

#include <QWidget>

namespace Ui {
class FavoritesPage;
}

class FavoritesPage : public QWidget
{
    Q_OBJECT

public:
    explicit FavoritesPage(QWidget *parent = nullptr);
    ~FavoritesPage();

private:
    Ui::FavoritesPage *ui;
};

#endif // FAVORITESPAGE_H
