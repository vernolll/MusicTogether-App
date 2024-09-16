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


#include "../ui/ui_mainwindow.h"
#include "music.h"

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
    void get_track();
    void binaryReceived(const QByteArray message);

private:
    Ui::MainWindow *ui;
    static QSqlTableModel* model;
    static QWebSocket *webSocket;
    static int room_id;
};

#endif // ROOM_PAGE_H
