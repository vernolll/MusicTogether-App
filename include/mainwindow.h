#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QStyle>

#include "autorization.h"
#include "rooms.h"
#include "main_page.h"
#include "room_page.h"


class Autorization;
class Rooms;
class Main_page;
class Room_page;


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
    void on_pushButton_create_room_clicked();
    void on_pushButton_connect_to_room_clicked();
    void on_tableView_rooms_doubleClicked(const QModelIndex &index);
    void on_pushButton_exit_clicked();
    void on_pushButton_playlist_clicked();
    void on_pushButton_add_mus_clicked();
    void on_pushButton_del_clicked();
    void on_pushButton_exit_room_clicked();
    void on_pushButton_back_2_clicked();
    void on_pushButton_synchron_clicked();
    void on_horizontalSlider_volume_sliderMoved(int position);
    void on_horizontalSlider_music_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    Autorization *autoriz;
    Rooms *room;
    Main_page *main_page;
    Room_page *room_page;
};
#endif // MAINWINDOW_H
