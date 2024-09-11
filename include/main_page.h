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
#include <QWebSocket>
#include <QPair>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>

#include "../ui/ui_mainwindow.h"

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

private:
    Ui::MainWindow *ui;
    QList<QPair<QString, QString>> table_values;
    QList<QPair<int, QString>> table_values1;
    QString room_name;
    QVector<int> id;
    int current_id;
    QSqlTableModel* model;
    QSqlDatabase db;

    void draw_table();
    void draw_table_users();
};

#endif // MAIN_PAGE_H
