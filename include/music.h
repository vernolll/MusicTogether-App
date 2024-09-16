#ifndef MUSIC_H
#define MUSIC_H

#include <QObject>
#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QtWebSockets>

#include "../ui/ui_mainwindow.h"

namespace Ui
{
class MainWindow;
}

class Music : public QObject
{
    Q_OBJECT
public:
    explicit Music(Ui::MainWindow *ui, QObject *parent = nullptr);
    static void get_room_id(int id);
    ~Music();

public slots:
    void show_playlist();
    void add_track();

private:
    Ui::MainWindow *ui;
    int roomId;
    QWebSocket *webSocket;
};

#endif // MUSIC_H
