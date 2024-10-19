#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

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
#include <QTableView>
#include <QtSql/QSqlTableModel>
#include <QPair>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>

#include "../ui/ui_mainwindow.h"
#include "room_page.h"
#include "qlabel_clicked.h"

class Room_page;
class ClickedLabel;

namespace Ui
{
class MainWindow;
}

class Main_page : public QObject
{
    Q_OBJECT
public:
    explicit Main_page(Ui::MainWindow *ui, QObject *parent = nullptr);
    void get_info();
    void connect_to_database();
    ~Main_page();

public slots:
    void switch_to_room(const QModelIndex &index);
    void back_to_main();
    void exit_from_acconunt();
    void online_users();
    void room_delete();

private:
    Ui::MainWindow *ui;
    ClickedLabel *clickedLabel;
    QList<QPair<QString, QString>> table_values;
    QList<QPair<int, QString>> table_values1;
    QString room_name;
    QVector<int> id;
    int current_id;
    QSqlTableModel* model;
    QSqlDatabase db;
    Room_page room_p;

    void draw_table();
};

#endif // MAIN_PAGE_H
