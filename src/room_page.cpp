#include "../include/room_page.h"


Room_page::Room_page(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{
    webSocket = new QWebSocket();

    connect(webSocket, &QWebSocket::connected, this, &Room_page::onConnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &Room_page::onTextMessageReceived);
    connect(webSocket, &QWebSocket::disconnected, this, &Room_page::onDisconnected);
    connect(webSocket, &QWebSocket::errorOccurred, this, &Room_page::onError);
}


int Room_page::room_id = 0;
QSqlTableModel *Room_page::model = nullptr;
QWebSocket *Room_page::webSocket = nullptr;


Room_page::~Room_page()
{
}


void Room_page::draw_table_users(int current_room, Ui::MainWindow *ui)
{
    room_id = current_room;
    online_users();
    model = new QSqlTableModel();

    if(!model)
    {
        qDebug() << "Error creating QSqlTableModel";
        return;
    }

    model->setQuery("SELECT * FROM OnlineUsers");
    model->setTable("OnlineUsers");

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

    ui->tableView_users_online->setModel(model);

    if(!ui->tableView_users_online)
    {
        qDebug() << "Error: tableView_users_online is NULL";
        delete model;
        return;
    }

    ui->tableView_users_online->hideColumn(0);
    ui->tableView_users_online->setColumnWidth(1, 256);
    ui->tableView_users_online->show();
}


void Room_page::online_users()
{
    QString link = "ws://localhost:8000/room?id=" + QString::number(room_id);
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
    request.setRawHeader("Sec-WebSocket-Protocol", token.toUtf8());
    webSocket->open(request);
}


void Room_page::onConnected()
{
    qDebug() << "WebSocket connected";
    sendEmptyJsonMessage();
}


void Room_page::onTextMessageReceived(const QString &message)
{
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing JSON message:" << error.errorString();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("type") && jsonObj["type"].toInt() == 1)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        QString event = dataObj["event"].toString();
        int userId = dataObj["userID"].toInt();

        if (event == "connected")
        {
            qDebug() << "User logged in with ID:" << userId;
        }
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 2)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        QString event = dataObj["event"].toString();
        int userId = dataObj["user_id"].toInt();

        if (event == "user logged out")
        {
            qDebug() << "User logged out with ID:" << userId;
        }
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 3)
    {
        QJsonArray dataArray = jsonObj["data"].toArray();
        for (const QJsonValue& value : dataArray)
        {
            int id = value.toInt();
            qDebug() << "ID of users:" << id;
        }
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 4)
    {
        qDebug() << message;
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 5)
    {
        QMessageBox::information(nullptr, "Уведомление", "Трек включен.");
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 6)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        int track_id = dataObj["trackID"].toInt();
        int time = dataObj["time"].toInt();
        QString path = dataObj["path"].toString();
        play_music(time, path);
    }
    else
    {
        qDebug() << message;
    }
}


void Room_page::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket error:" << error;
}


void Room_page::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
}


void Room_page::disconnecting()
{
    if (webSocket && webSocket->isValid()) {
        webSocket->close();
    } else {
        qDebug() << "Error: WebSocket is not valid or null.";
    }
}


void Room_page::leaving_room()
{
    disconnecting();
    ui->tableView_users_online->setVisible(false);
    ui->pushButton_playlist->setVisible(false);
    ui->label_room->setVisible(false);
    ui->label_music->setVisible(false);
    ui->pushButton_exit_room->setVisible(false);

}


void Room_page::play_music(int time, QString path)
{
    path = "http://localhost:8000/" + path;
    qDebug() << path;

    QMediaPlayer *player = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromUserInput(path));
    audioOutput->setVolume(50);

    player->play();
}


void Room_page::sendEmptyJsonMessage()
{
    QJsonObject jsonMessage;
    jsonMessage["type"] = 3;
    jsonMessage["data"] = QJsonObject();

    QJsonDocument jsonDoc(jsonMessage);
    QString jsonString = QString(jsonDoc.toJson());

    if(webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
    {
        webSocket->sendTextMessage(jsonString);
    }
    else
    {
        qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
    }
}


void Room_page::send_playing(const QModelIndex &index)
{
    if (!isPlay)
    {
        QJsonObject jsonMessage;
        jsonMessage["type"] = 6;

        QJsonObject dataObject;
        dataObject["trackID"] = 10;
        dataObject["time"] = 0.1;

        jsonMessage["data"] = dataObject;

        if (webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
        {
            QJsonDocument jsonDoc(jsonMessage);
            QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
            webSocket->sendTextMessage(QString::fromUtf8(jsonData));
        }
        else
        {
            qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
        }
    }
    else
    {
        isPlay = false;
        qDebug() << "Stop music";
    }
}



void Room_page::get_tracks_list()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QString link = "http://localhost:8000/tracks/room/" + QString::number(room_id);
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
        return;
    }

    request.setRawHeader("Authorization", token.toUtf8());

    QNetworkReply *reply = manager.get(request);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

        QJsonArray tracksArray = jsonResponse.array();

        QSqlQuery query;
        query.exec("DROP TABLE IF EXISTS tracks");
        query.exec("CREATE TABLE IF NOT EXISTS tracks (id INTEGER PRIMARY KEY, artist TEXT, title TEXT)");

        if (tracksArray.isEmpty())
        {
            qDebug() << "No tracks found in the response.";
            draw_table_tracks();
            reply->deleteLater();
            return;
        }

        for (const auto& trackValue : tracksArray)
        {
            QJsonObject trackObject = trackValue.toObject();
            int trackId = trackObject["id"].toInt();
            QString artist = trackObject["artist"].toString();
            QString title = trackObject["title"].toString();

            artist.replace('\n',' ');

            qDebug() << trackId << artist << title;

            query.prepare("INSERT INTO tracks (id, artist, title) VALUES (:id, :artist, :title)");
            query.bindValue(":id", trackId);
            query.bindValue(":artist", artist);
            query.bindValue(":title", title);

            if (!query.exec())
            {
                QSqlError error = query.lastError();
                qDebug() << "Error inserting data into the table:" << error.text();
            }
        }

        draw_table_tracks();
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
        qDebug() << "Network Error: " << reply->errorString() << "getting list of tracks";
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }

    reply->deleteLater();
}


void Room_page::draw_table_tracks()
{
    model1 = new QSqlTableModel();

    if(!model1)
    {
        qDebug() << "Error creating QSqlTablemodel";
        return;
    }

    model1->setQuery("SELECT * FROM tracks");
    model1->setTable("tracks");

    if(model1->lastError().isValid())
    {
        qDebug() << "Error in SQL query: " << model1->lastError().text();
        delete model1;
        return;
    }

    model1->select();

    if(model1->lastError().isValid())
    {
        qDebug() << "Error executing query: " << model1->lastError().text();
        delete model1;
        return;
    }

    ui->tableView_music->setModel(model1);

    if(!ui->tableView_music)
    {
        qDebug() << "Error: tableView_music is NULL";
        delete model1;
        return;
    }

    ui->tableView_music->hideColumn(0);
    ui->tableView_music->setColumnWidth(1, 305);
    ui->tableView_music->setColumnWidth(2, 305);
    ui->tableView_music->show();
}


void Room_page::show_playlist()
{
    ui->stackedWidget->setCurrentWidget(ui->page_playlist);
    get_tracks_list();
}


void Room_page::add_track()
{
    QString artist = ui->lineEdit_singer->text();
    QString music = ui->lineEdit_music_name->text();

    if(!artist.isEmpty() && !music.isEmpty() && room_id != 0)
    {
        QNetworkAccessManager manager;
        QEventLoop loop;

        connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QUrl url("http://localhost:8000/tracks");
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

        QJsonObject trackObject;
        trackObject["title"] = music;
        trackObject["artist"] = artist;
        trackObject["room_id"] = room_id;

        QJsonDocument trackDoc(trackObject);
        QByteArray postData = trackDoc.toJson();

        QNetworkReply *reply = manager.post(request, postData);

        loop.exec();

        if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 201)
        {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            QJsonObject jsonObject = jsonDoc.object();

            if (jsonObject.contains("id"))
            {
                int trackId = jsonObject["id"].toInt();
                qDebug() << "Track created with id: " << trackId;
            }
            else
            {
                qDebug() << "No 'id' field found in the response";
            }

            get_tracks_list();
        }
        else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400)
        {
            QMessageBox::warning(nullptr, "Ошибка", "Плохой запрос.");
        }
        else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401)
        {
            QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы.");
        }
        else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404)
        {
            QMessageBox::warning(nullptr, "Ошибка", "Трек не найден.");
        }
        else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
        {
            QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");

        }
        else
        {
            qDebug() << "Network Error: " << reply->errorString() << "(add_track)";
            qDebug() <<  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        }
        reply->deleteLater();
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не заполнили поля.");
    }
}
