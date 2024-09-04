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
//#include <QAbstractTableModel>
#include <QPair>

#include "../ui/ui_mainwindow.h"
#include "customtablemodel.h"


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
    ~Main_page();

public slots:
    void switch_to_room(const QModelIndex &index);
    void back_to_main();
    void exit_from_acconunt();

private:
    Ui::MainWindow *ui;
    QList<QPair<QString, QString>> table_values;

    void draw_table();
};

#endif // MAIN_PAGE_H
