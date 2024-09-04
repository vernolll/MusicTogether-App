#ifndef AUTORIZATION_H
#define AUTORIZATION_H

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

#include "../ui/ui_mainwindow.h"
#include "main_page.h"

class Main_page;

namespace Ui
{
class MainWindow;
}


class Autorization : public QObject
{
    Q_OBJECT
public:
    explicit Autorization(Ui::MainWindow *ui, QObject *parent = nullptr);
    ~Autorization();

public slots:
    void switch_to_registr();
    void registration();
    void main_page();

private:
    Ui::MainWindow *ui;
    Main_page *main_page1;

    bool isPasswordStrong(const QString &password);
};

#endif // AUTORIZATION_H
