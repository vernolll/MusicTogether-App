#include "../include/room_page.h"


Room_page::Room_page(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{
    webSocket = new QWebSocket();

    ui->horizontalSlider_volume->setRange(0, 100);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Room_page::getCurrentSongPosition);

    connect(webSocket, &QWebSocket::connected, this, &Room_page::onConnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &Room_page::onTextMessageReceived);
    connect(webSocket, &QWebSocket::disconnected, this, &Room_page::onDisconnected);
    connect(webSocket, &QWebSocket::errorOccurred, this, &Room_page::onError);
}


int Room_page::room_id = 0;
QWebSocket *Room_page::webSocket = nullptr;


Room_page::~Room_page()
{
}

extern QString server_path;


void Room_page::draw_table_users()
{
    QSqlQuery query;
    if (!query.exec("SELECT * FROM OnlineUsers"))
    {
        qDebug() << "Error executing SQL query: " << query.lastError().text();
        return;
    }

    if (ui->tableWidget_users->columnCount() == 0)
    {
        ui->tableWidget_users->setColumnCount(1);
    }

    ui->tableWidget_users->setColumnCount(1);
    ui->tableWidget_users->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_users->setRowCount(0);
    ui->tableWidget_users->horizontalHeader()->setStretchLastSection(true);

    int row = 0;
    while (query.next())
    {
        QString username = query.value(1).toString();
        QString photoPath = query.value(2).toString();

        QWidget* itemWidget = new QWidget(ui->tableWidget_users);
        QHBoxLayout* layout = new QHBoxLayout(itemWidget);

        QLabel* nameLabel = new QLabel(username);
        QLabel* photoLabel = new QLabel();

        nameLabel->setMinimumSize(70, 20);
        photoLabel->setFixedSize(20, 20);

        if (photoPath.startsWith("http"))
        {
            QPixmap pixmap;
            if (loadImageFromUrl(photoPath, pixmap))
            {
                photoLabel->setPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else
            {
                qDebug() << "Failed to load photo from URL" << photoPath;
            }
        }
        else
        {
            QPixmap pixmap(photoPath);
            if (!pixmap.isNull())
            {
                photoLabel->setPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else
            {
                qDebug() << "Failed to load photo from local path" << photoPath;
            }
        }

        layout->addWidget(photoLabel);
        layout->addWidget(nameLabel);

        itemWidget->setLayout(layout);
        ui->tableWidget_users->insertRow(row);
        ui->tableWidget_users->setCellWidget(row, 0, itemWidget);
        ++row;
    }

    ui->tableWidget_users->horizontalHeader()->setVisible(false);
    ui->tableWidget_users->update();
    ui->tableWidget_users->setVisible(true);
}


bool Room_page::loadImageFromUrl(const QString& url, QPixmap& pixmap)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        pixmap.loadFromData(data);
        reply->deleteLater();
        return true;
    }
    else
    {
        qDebug() << "Failed to download image:" << reply->errorString();
        reply->deleteLater();
        return false;
    }
}


void Room_page::connecthion_to_websocket(int roomID)
{
    room_id = roomID;
    QString link = "ws://" + server_path + "/room?id=" + QString::number(room_id);
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

    draw_table_users();
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

    if(jsonObj.contains("type") && jsonObj["type"].toInt() == 0)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Трек не готов к воспроизведению. Это займет не более двух минут.");
    }
    else if (jsonObj.contains("type") && jsonObj["type"].toInt() == 1)
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
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 5 && jsonObj.contains("event") && jsonObj["event"].toString() == "ready")
    {
        QMessageBox::information(nullptr, "Уведомление", "Трек готов к воспроизведению.");
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 5 && jsonObj.contains("event") && jsonObj["event"].toString() == "remove")
    {
        get_tracks_list();
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 6)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        int time = dataObj["time"].toInt();
        if(path_mus == "http://" + server_path + "/" + dataObj["path"].toString())
        {
            player->play();
        }
        else
        {
            path_mus = dataObj["path"].toString();
            play_music(time);
        }
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 7)
    {
        player->pause();
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 8)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        int new_time = dataObj["time"].toInt();
        rewind_msuic(new_time);
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 9)
    {
        QJsonObject dataObj = jsonObj["data"].toObject();
        int new_time = dataObj["time"].toInt();
        rewind_msuic(new_time);
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toInt() == 11)
    {
        get_tracks_list();
        QJsonObject dataObj = jsonObj["data"].toObject();
        QJsonObject trackObj = dataObj["track"].toObject();

        path_mus = trackObj["path"].toString();
        trackID = trackObj["id"].toInt();
        int mus_time = dataObj["time"].toInt();
        mus_status = dataObj["status"].toString();
        qDebug() << "track" << path_mus;
        qDebug() << mus_status;
        qDebug() << mus_time;
        i_am_new(path_mus, mus_time, mus_status);
    }
    else
    {
        qDebug() << message;
    }
}


void Room_page::i_am_new(QString track, int mus_time, QString mus_status)
{
    if(mus_status == "playing")
    {
        int rowNumber;
        QSqlQuery query;
        query.prepare("SELECT ROW_NUMBER() OVER (ORDER BY id) AS row_num FROM tracks WHERE id = :trackid");
        query.bindValue(":trackid", trackID);
        if (query.exec())
        {
            if (query.next())
            {
                rowNumber = query.value(0).toInt();
            } else
            {
                qDebug() << "No row with id = 3 found.";
            }
        }
        else
        {
            qDebug() << "Error executing query:" << query.lastError().text();
        }

        QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(rowNumber - 1, 0));
        ui->pushButton_play->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));

        if (button)
        {
            button->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
            button->update();
        }
        else
        {
            qDebug() << "button is null";
        }
        play_music(mus_time);
    }
    else if(mus_status == "paused")
    {
        int rowNumber;
        QSqlQuery query;
        query.prepare("SELECT ROW_NUMBER() OVER (ORDER BY id) AS row_num FROM tracks WHERE id = :trackid");
        query.bindValue(":trackid", trackID);
        if (query.exec())
        {
            if (query.next())
            {
                rowNumber = query.value(0).toInt();
            } else
            {
                qDebug() << "No row with id = 3 found.";
            }
        }
        else
        {
            qDebug() << "Error executing query:" << query.lastError().text();
        }

        QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(rowNumber - 1, 0));
        ui->pushButton_play->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));

        if (button)
        {
            button->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
            button->update();
        }
        else
        {
            qDebug() << "button is null";
        }


        player = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);

        player->setAudioOutput(audioOutput);
        player->setSource(QUrl::fromUserInput(path_mus));
        audioOutput->setVolume(ui->horizontalSlider_volume->value());

        connect(player, &QMediaPlayer::mediaStatusChanged, this, [mus_time, this](QMediaPlayer::MediaStatus status)
                {
                    if (status == QMediaPlayer::LoadedMedia)
                    {
                        player->setPosition(mus_time);
                        qDebug() << "new position:" << player->position();
                        isPlay = false;

                        ui->horizontalSlider_music->setRange(0, player->duration());
                        ui->horizontalSlider_music->setEnabled(true);
                    }
                });
    }
    else if(mus_status == "stopped")
    {

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
    if (webSocket && webSocket->isValid())
    {
        webSocket->close();
    } else
    {
        qDebug() << "Error: WebSocket is not valid or null.";
    }
    delete audioOutput;
    delete player;
}


void Room_page::leaving_room()
{
    if(!player->StoppedState)
    {
        player->stop();
    }

    disconnecting();
    ui->tableWidget_users->setVisible(false);
    ui->pushButton_playlist->setVisible(false);
    ui->label_room->setVisible(false);
    ui->label_music->setVisible(false);
    ui->pushButton_exit_room->setVisible(false);
    ui->pushButton_synchron->setVisible(false);
    timer->stop();
}


void Room_page::play_music(int start_time)
{
    path_mus = "http://" + server_path + "/" + path_mus;
    qDebug() << path_mus;

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromUserInput(path_mus));
    audioOutput->setVolume(ui->horizontalSlider_volume->value());

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [start_time, this](QMediaPlayer::MediaStatus status)
            {
                if (status == QMediaPlayer::LoadedMedia)
                {
                    player->setPosition(start_time);
                    player->play();
                    isPlay = true;

                    timer->start(2000);
                    ui->horizontalSlider_music->setRange(0, player->duration());
                    ui->horizontalSlider_music->setEnabled(true);
                }
            });
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


void Room_page::send_playing(int index, QPushButton *button)
{
    if (!isPlay)
    {
        isPlay = true;
        QJsonObject jsonMessage;
        jsonMessage["type"] = 6;


        int new_trackID = index;

        if(new_trackID == trackID)
        {
            QJsonObject dataObject;
            dataObject["trackID"] = new_trackID;
            dataObject["time"] = player->position();
            qDebug() << player->position();
            jsonMessage["data"] = dataObject;
        }
        else
        {
            QJsonObject dataObject;
            dataObject["trackID"] = new_trackID;
            dataObject["time"] = 0;
            jsonMessage["data"] = dataObject;
            trackID = new_trackID;
        }

        if (webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
        {
            QJsonDocument jsonDoc(jsonMessage);
            QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
            webSocket->sendTextMessage(QString::fromUtf8(jsonData));

            button->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
            ui->pushButton_play->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
            button->update();
        }
        else
        {
            qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
            qDebug() << webSocket->isValid() << webSocket->state();
        }
    }
    else
    {
        isPlay = false;

        QJsonObject jsonMessage;
        jsonMessage["type"] = 7;
        QJsonObject dataObject;
        jsonMessage["data"] = QJsonObject();
        QJsonDocument jsonDoc(jsonMessage);
        QString jsonString = QString(jsonDoc.toJson());

        if(webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
        {
            qDebug() << "Paused music";
            webSocket->sendTextMessage(jsonString);
            timer->stop();
            button->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
            ui->pushButton_play->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
            button->update();
        }
        else
        {
            qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
        }
    }
}


void Room_page::getCurrentSongPosition()
{
    QJsonObject jsonMessage;
    jsonMessage["type"] = 10;

    QJsonObject dataObject;
    dataObject["time"] = player->position();
    jsonMessage["data"] = dataObject;

    QJsonDocument jsonDoc(jsonMessage);
    QString jsonString = QString(jsonDoc.toJson());

    if (webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << player->position();
        webSocket->sendTextMessage(jsonString);
        ui->horizontalSlider_music->setSliderPosition(player->position());
    }
    else
    {
        qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
    }
}


void Room_page::send_rewind(int new_time)
{
    QJsonObject jsonMessage;
    QJsonObject dataObject;
    jsonMessage["type"] = 8;
    jsonMessage["data"] = QJsonObject();
    dataObject["time"] = new_time;

    QJsonDocument jsonDoc(jsonMessage);
    QString jsonString = QString(jsonDoc.toJson());

    if(webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Rewinding";
        webSocket->sendTextMessage(jsonString);
    }
    else
    {
        qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
    }
}


void Room_page::rewind_msuic(int new_time)
{
    qDebug() << "new time:" << new_time;
    if (player) delete player;
    if (audioOutput) delete audioOutput;

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromUserInput(path_mus));
    audioOutput->setVolume(ui->horizontalSlider_volume->value());

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [new_time, this](QMediaPlayer::MediaStatus status)
            {
                if (status == QMediaPlayer::LoadedMedia)
                {
                    player->setPosition(new_time);
                    player->play();
                    isPlay = true;

                    timer->start(2000);
                    ui->horizontalSlider_music->setRange(0, player->duration());
                    ui->horizontalSlider_music->setEnabled(true);
                    send_rewind(new_time);
                }
            });
}


void Room_page::get_tracks_list()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QString link = "http://" + server_path + "/tracks/room/" + QString::number(room_id);
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
        query.exec("DELETE FROM sqlite_sequence WHERE name = 'tracks'");
        query.exec("CREATE TABLE IF NOT EXISTS tracks (id INTEGER PRIMARY KEY, artist TEXT, title TEXT)");
        query.exec("ALTER TABLE tracks ADD COLUMN delete_button TEXT");

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
        qDebug() << reply->readAll();
    }

    reply->deleteLater();
}


void Room_page::draw_table_tracks()
{
    model1 = new QSqlTableModel();

    if (!model1)
    {
        qDebug() << "Error creating QSqlTablemodel";
        return;
    }

    model1->setQuery("SELECT * FROM tracks");
    model1->setTable("tracks");

    if (model1->lastError().isValid())
    {
        qDebug() << "Error in SQL query: " << model1->lastError().text();
        delete model1;
        return;
    }

    model1->select();

    if (model1->lastError().isValid())
    {
        qDebug() << "Error executing query: " << model1->lastError().text();
        delete model1;
        return;
    }

    ui->tableWidget->setRowCount(model1->rowCount());
    ui->tableWidget->setColumnCount(model1->columnCount());

    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->hide();

    for (int row = 0; row < model1->rowCount(); ++row)
    {
        for (int col = 0; col < model1->columnCount(); ++col)
        {
            QTableWidgetItem *item = new QTableWidgetItem(model1->data(model1->index(row, col)).toString());
            ui->tableWidget->setItem(row, col, item);

            if (col == 0)
            {
                QPushButton *button = new QPushButton();
                button->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
                ui->tableWidget->setCellWidget(row, col, button);

                connect(button, &QPushButton::clicked, this, [this, row, button]()
                        {
                            QVariant idVariant = model1->data(model1->index(row, 0));
                            int id = idVariant.toInt();
                            send_playing(id, button);
                });
            }
            else if(col == 3)
            {
                QPushButton *button_del = new QPushButton();
                button_del->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton));
                ui->tableWidget->setCellWidget(row, col, button_del);

                connect(button_del, &QPushButton::clicked, this, [this, row]()
                        {
                            QVariant idVariant = model1->data(model1->index(row, 0));
                            int id = idVariant.toInt();
                            del_music(id);
                        });
            }
        }
    }

    ui->tableWidget->setColumnWidth(0, 10);
    ui->tableWidget->setColumnWidth(1, 285);
    ui->tableWidget->setColumnWidth(2, 260);
    ui->tableWidget->setColumnWidth(3, 10);
    ui->tableWidget->show();
}


void Room_page::del_music(int musId)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QString link = "http://" + server_path + "/tracks/delete/" + QString::number(musId);
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

    QNetworkReply *reply = manager.deleteResource(request);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 204)
    {
        QByteArray responseData = reply->readAll();
        qDebug() << "Delete request succeeded:" << responseData;
        get_tracks_list();
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
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }

    reply->deleteLater();
}


void Room_page::show_playlist()
{
    ui->stackedWidget->setCurrentWidget(ui->page_playlist);
    get_tracks_list();
}


void Room_page::send_synchron()
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


void Room_page::add_track()
{
    QString artist = ui->lineEdit_singer->text();
    QString music = ui->lineEdit_music_name->text();

    if(!artist.isEmpty() && !music.isEmpty() && room_id != 0)
    {
        QNetworkAccessManager manager;
        QEventLoop loop;

        connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QUrl url("http://" + server_path + "/tracks");
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
            qDebug() << reply->readAll();
        }
        reply->deleteLater();
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не заполнили поля.");
    }
}


void Room_page::setting_volume(int volume)
{
    qreal volumeLevel = (qreal)volume / 100.0;

    audioOutput->setVolume(volumeLevel);
    qDebug() << "new volume:" << volumeLevel;
    audioOutput->setVolume(volumeLevel);
}


void Room_page::playbutton()
{
    int rowNumber;
    QSqlQuery query;
    query.prepare("SELECT ROW_NUMBER() OVER (ORDER BY id) AS row_num FROM tracks WHERE id = :trackid");
    query.bindValue(":trackid", trackID);
    if (query.exec())
    {
        if (query.next())
        {
            rowNumber = query.value(0).toInt();
        } else
        {
            qDebug() << "No row with id = 3 found.";
        }
    }
    else
    {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(rowNumber - 1, 0));
    send_playing(trackID, button);
}
