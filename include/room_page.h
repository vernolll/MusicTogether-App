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
    static void disconnecting();
    static void online_users();
    void sendEmptyJsonMessage();
    ~Room_page();

public slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();
    void show_playlist();
    void add_track();
    void send_playing(const QModelIndex &index);
    void leaving_room();

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

    void get_tracks_list();
    void draw_table_tracks();
    void play_music(int time, QString path);
};

#endif // ROOM_PAGE_H
