#include "../include/music.h"

Music::Music(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{
}


Music::~Music()
{
}


void Music::get_room_id(int id)
{
    QFile file("room.txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << id;
        file.close();
    }
    else
    {
        qDebug() << "Error opening file for writing.";
    }
}


void Music::show_playlist()
{
    ui->stackedWidget->setCurrentWidget(ui->page_playlist);

    //ораганизовать подгрузку данных о плейлисте
}


void Music::add_track()
{
    QFile file("room.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString roomIdStr = in.readAll();
        roomId = roomIdStr.toInt();
        qDebug() << "Read roomId from file: " << roomId;

        file.close();
    }
    else
    {
        qDebug() << "Failed to open room.txt for reading";
    }

    QString artist = ui->lineEdit_singer->text();
    QString music = ui->lineEdit_music_name->text();

    if(!artist.isEmpty() && !music.isEmpty() && roomId != 0)
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
        trackObject["room_id"] = roomId;

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
            qDebug() << "Network Error: " << reply->errorString();
            QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        }
        reply->deleteLater();
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не заполнили поля.");
    }
}
