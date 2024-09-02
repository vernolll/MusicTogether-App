#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "autorization.h"

class Autorization;


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void on_pushButton_log_in_3_clicked();
    void on_pushButton_registr_3_clicked();
    void on_pushButton_registration_2_clicked();

private:
    Ui::MainWindow *ui;
    Autorization *autoriz;
};
#endif // MAINWINDOW_H
