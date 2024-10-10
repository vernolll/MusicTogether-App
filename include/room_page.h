#ifndef ROOM_PAGE_H
#define ROOM_PAGE_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QFile>
#include <QtWebSockets>
#include <QMessageBox>
#include <QIODevice>
#include <QAudioOutput>
#include <QBuffer>
#include <QAudioFormat>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QTime>
#include <QStyle>


#include "../ui/ui_mainwindow.h"

class Music;

namespace Ui
{
class MainWindow;
}


class Room_page : public QObject
{
    Q_OBJECT
public:
    explicit Room_page(Ui::MainWindow *ui, QObject *parent = nullptr);
    static void draw_table_users(int current_room, Ui::MainWindow *ui);
    void disconnecting();
    void connecthion_to_websocket(int room_id);
    void sendEmptyJsonMessage();
    ~Room_page();

public slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();
    void show_playlist();
    void add_track();
    void send_playing(int index, QPushButton *button);
    void leaving_room();
    void send_rewind();
    void send_synchron();
    void getCurrentSongPosition();
    void setting_volume(int volume);
    void rewind_msuic(int new_time);
    void del_music(int musId, QPushButton *del_button);

private:
    Ui::MainWindow *ui;
    static QSqlTableModel* model;
    QSqlTableModel* model1;
    static QWebSocket *webSocket;
    static int room_id;
    QMediaPlayer* mediaPlayer;
    QByteArray mus_msgs;
    QAudioOutput* audioOutput;
    bool isPlay;
    QMediaPlayer *player;
    int trackID;
    int new_time;
    QTimer *timer;
    QString path_mus;

    void get_tracks_list();
    void draw_table_tracks();
    void play_music(int time);

};
#endif // ROOM_PAGE_H
