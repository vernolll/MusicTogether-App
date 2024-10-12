#include "../include/main_page.h"


Main_page::Main_page(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui),
    room_p(ui)
{
}


Main_page::~Main_page()
{
}


extern QString server_path;


void Main_page::connect_to_database()
{
    QString dbFilePath = "build";
    db = QSqlDatabase::addDatabase("QSQLITE");

    if (!QFile::exists(dbFilePath))
    {
        QFile file(dbFilePath);
        if (file.open(QIODevice::WriteOnly))
        {
            file.close();
        }
        else
        {
            qDebug() << "Error: Unable to create database file";
            return;
        }
    }

    db.setDatabaseName(dbFilePath);

    if (!db.open())
    {
        qDebug() << "Error: Unable to open database";
        return;
    }
}


void Main_page::get_info()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url("http://" + server_path + "/rooms/my");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QFile file("token.txt");
    QString token;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        token = in.readAll();
        file.close();
    }
    else if(!file.exists() || file.size() == 0)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованны.");
    }

    request.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

        QJsonObject mainObject = jsonResponse.object();
        QJsonArray roomsArray = mainObject["rooms"].toArray();

        table_values.clear();
        id.clear();
        table_values.resize(0);
        id.resize(0);
        foreach (const QJsonValue& roomValue, roomsArray)
        {
            QJsonObject roomObject = roomValue.toObject();
            QString roomName = roomObject["name"].toString();
            QString roomCode = roomObject["code"].toString();

            id.push_back(roomObject["id"].toInt());
            table_values.append(QPair<QString, QString>(roomName, roomCode));
        }

        QSqlQuery query;
        query.exec("DROP TABLE IF EXISTS Rooms");
        query.exec("CREATE TABLE IF NOT EXISTS Rooms (id INTEGER PRIMARY KEY, name TEXT, code TEXT)");

        for (int i = 0; i < table_values.size(); ++i)
        {
            query.prepare("INSERT INTO Rooms (id, name, code) VALUES (:id, :name, :code)");
            query.bindValue(":id", id[i]);
            query.bindValue(":name", table_values[i].first);
            query.bindValue(":code", table_values[i].second);

            if (!query.exec())
            {
                QSqlError error = query.lastError();
                qDebug() << "Error inserting data into the table:" << error.text();
            }
        }

        draw_table();
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");
    }
    else
    {
        qDebug() << "Network Error: " << reply->errorString();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }
    reply->deleteLater();
}


void Main_page::draw_table()
{
    if(!db.isOpen())
    {
        db.open();
    }

    model = new QSqlTableModel();

    if(!model)
    {
        qDebug() << "Error creating QSqlTableModel";
        return;
    }

    model->setQuery("SELECT * FROM Rooms");
    model->setTable("Rooms");

    if(model->lastError().isValid())
    {
        qDebug() << "Error in SQL query: " << model->lastError().text();
        delete model;
        return;
    }

    model->select();

    if(model->lastError().isValid())
    {
        qDebug() << "Error executing query: " << model->lastError().text();
        delete model;
        return;
    }

    ui->tableView_rooms->setModel(model);

    if(!ui->tableView_rooms)
    {
        qDebug() << "Error: tableView_rooms is NULL";
        delete model;
        return;
    }

    ui->tableView_rooms->hideColumn(0);
    ui->tableView_rooms->show();
}


void Main_page::switch_to_room(const QModelIndex &index)
{
    current_id = index.sibling(index.row(), 0).data().toInt();

    online_users();

    QString title;

    if (index.isValid())
    {
        title = "Команата: " + index.sibling(index.row(), 1).data().toString();
    }

    room_p.connecthion_to_websocket(current_id);
    ui->label_room->setText(title);
    ui->label_room->setVisible(true);
    ui->pushButton_playlist->setVisible(true);
    ui->pushButton_exit_room->setVisible(true);
    ui->label_music->setVisible(true);
    ui->pushButton_synchron->setVisible(true);
}


void Main_page::back_to_main()
{
    ui->stackedWidget->setCurrentWidget(ui->page_main);
}


void Main_page::room_delete()
{
    int index = ui->tableView_rooms->currentIndex().row();
    current_id = id[index];

    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QString link = "http://" + server_path + "/rooms/" + QString::number(current_id) + "/leave";
    QUrl url(link);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QFile file("token.txt");
    QString token;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        token = in.readAll();
        file.close();
    }
    else if(!file.exists() || file.size() == 0)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованны.");
    }

    request.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = manager.post(request, QByteArray());

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        qDebug() << "Вы покинули комнату.";
        get_info();
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 403)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Пользователь не в комнате.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Комната не найдена.");
        qDebug() << current_id;
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");
    }
    else
    {
        qDebug() << "Network Error: " << reply->errorString();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }
    reply->deleteLater();
}


void Main_page::exit_from_acconunt()
{
    table_values.clear();

    QFile file("token.txt");
    if (!file.exists() || file.size() == 0)
    {

        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.resize(0);
            file.close();
        }
        else
        {
            qDebug() << "Failed to create the file.";
        }

        ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);

    }
    else
    {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.resize(0);
            file.close();
        }
        else
        {
            qDebug() << "Failed to clear the file.";
        }

        QSqlQuery query;
        query.exec("DROP TABLE IF EXISTS Rooms");

        ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);
    }
}


struct UserData
{
    int id;
    QString username;
    QString photoData;
};


void Main_page::online_users()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QString link = "http://" + server_path + "/rooms/" + QString::number(current_id) + "/users";
    QUrl url(link);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QFile file("token.txt");
    QString token;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        token = in.readAll();
        file.close();
    }
    else if(!file.exists() || file.size() == 0)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованны.");
    }

    request.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

        QJsonObject mainObject = jsonResponse.object();
        QJsonArray usersArray = mainObject["users"].toArray();

        table_values1.clear();
        table_values1.resize(0);

        QList<UserData> usersList;

        foreach (const QJsonValue& roomValue, usersArray)
        {
            QJsonObject roomObject = roomValue.toObject();
            int userid = roomObject["id"].toInt();
            QString username = roomObject["username"].toString();
            QString photoData = roomObject["avatar"].toString();
            UserData userData;
            userData.id = userid;
            userData.username = username;
            userData.photoData = "http://" + server_path + "/" + photoData;

            usersList.append(userData);
        }

        QSqlQuery query;
        query.exec("DROP TABLE IF EXISTS OnlineUsers");
        query.exec("CREATE TABLE IF NOT EXISTS OnlineUsers (id INTEGER PRIMARY KEY, username TEXT, photopath TEXT)");

        for (const UserData& userData : usersList)
        {
            query.prepare("INSERT INTO OnlineUsers (id, username, photopath) VALUES (:id, :username, :photopath)");
            query.bindValue(":id", userData.id);
            query.bindValue(":username", userData.username);
            query.bindValue(":photopath", userData.photoData);

            if (!query.exec())
            {
                QSqlError error = query.lastError();
                qDebug() << "Error inserting data into the table:" << error.text();
            }
        }
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");
    }
    else
    {
        qDebug() << "Network Error: " << reply->errorString();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        qDebug() << reply->readAll();
    }
    reply->deleteLater();
}
