#ifndef ROOMS_H
#define ROOMS_H

#include <QDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QFile>
#include "main_page.h"


namespace Ui
{
class Rooms;
}


class Main_page;


class Rooms : public QDialog
{
    Q_OBJECT

public:
    explicit Rooms(QWidget *parent = nullptr);
    ~Rooms();

signals:
    void on_pushButton_conf_clicked();
    void callGetInfo();

public slots:
    void new_room();
    void connect_to_existed();

private slots:
    void creating_room();
    void connect_to_room(QString code);

private:
    Ui::Rooms *ui;
    bool is_new;
    Main_page* mainPage;
};

#endif // ROOMS_H
