#ifndef STYLEWIND_H
#define STYLEWIND_H

#include <QObject>
#include <QWidget>
#include "../ui/ui_mainwindow.h"

namespace Ui
{
class MainWindow;
}


class StyleWind : public QObject
{
    Q_OBJECT
public:
    StyleWind(Ui::MainWindow *ui, QObject *parent = nullptr);
    ~StyleWind();

    void setStyleWind();

private:
    Ui::MainWindow *ui;
};

#endif // STYLEWIND_H
