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


namespace Ui
{
class MainWindow;
}


class Room_page : public QObject
{
    Q_OBJECT
public:
    explicit Room_page(Ui::MainWindow *ui, QObject *parent = nullptr);
    void draw_table_users(int current_room);
    void disconnecting();
    ~Room_page();

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();

private:
    Ui::MainWindow *ui;
    QSqlTableModel* model;
    QWebSocket *webSocket;
    int room_id;

    void online_users();
    void sendEmptyJsonMessage();
};

#endif // ROOM_PAGE_H
