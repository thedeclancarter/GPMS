#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <QWidget>

namespace Ui {
class ProjectPage;
}

class ProjectPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectPage(QWidget *parent = nullptr);
    ~ProjectPage();

private:
    Ui::ProjectPage *ui;
};

#endif // PROJECTPAGE_H
