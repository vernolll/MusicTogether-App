#include "../include/rooms.h"
#include "../ui/ui_rooms.h"


Rooms::Rooms(QWidget *parent, Main_page* mainPage)
    : QDialog(parent)
    , ui(new Ui::Rooms)
    , mainPage(mainPage)
{
    ui->setupUi(this);
    setWindowTitle("Музыка");

    connect(this, SIGNAL(on_pushButton_conf_clicked()), this, SLOT(creating_room()));
    connect(this, &Rooms::callGetInfo, mainPage, &Main_page::get_info);

    styling();
}


Rooms::~Rooms()
{
    delete ui;
}


void Rooms::new_room()
{
    this->show();
    is_new = true;
}

extern QString server_path;

void Rooms::creating_room()
{
    QString roomCode;

    if(is_new)
    {
        QString name = ui->lineEdit_room_name->text();

        if(!name.isEmpty())
        {
            QNetworkAccessManager manager;
            QEventLoop loop;

            connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

            QUrl url("http://" + server_path + "/rooms");
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

            request.setRawHeader("Authorization", token.toUtf8());

            QJsonObject jsonObject;
            jsonObject["name"] = name;
            QJsonDocument jsonDoc(jsonObject);
            QByteArray jsonData = jsonDoc.toJson();

            QNetworkReply *reply = manager.post(request, jsonData);

            loop.exec();

            if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 201)
            {
                QByteArray responseData = reply->readAll();
                QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

                QJsonObject jsonObject = jsonResponse.object();
                this->close();
                roomCode = jsonObject["code"].toString();

                emit callGetInfo();
            }
            else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400)
            {
                QMessageBox::warning(nullptr, "Ошибка", "Плохой запрос.");
            }
            else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
            {
                QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");
            }
            else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 409)
            {
                QMessageBox::warning(nullptr, "Ошибка", "Вы уже в этой комнате.");
            }
            else
            {
                qDebug() << "Error: " << reply->errorString();
                QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
                qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            }

            reply->deleteLater();
            this->close();
        }
        else
        {
            QMessageBox::warning(nullptr, "Ошибка", "Введите название комнаты.");
        }
    }
    else
    {
        roomCode = ui->lineEdit_room_name->text();
        if(!roomCode.isEmpty())
        {
            connect_to_room(roomCode);
        }
        else
        {
            QMessageBox::warning(nullptr, "Ошибка", "Введите название комнаты.");
        }
    }
}


void Rooms::connect_to_existed()
{
    this->show();
    is_new = false;
}


void Rooms::connect_to_room(QString code)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit, Qt::QueuedConnection);

    QUrl url("http://" + server_path + "/rooms/enter");
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

    request.setRawHeader("Authorization", token.toUtf8());

    QJsonObject jsonObject;
    jsonObject["code"] = code;
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();

    QNetworkReply *reply = manager.post(request, jsonData);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        this->close();
        emit callGetInfo();

    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Плохой запрос.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Комната не найдена.");
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }
    reply->deleteLater();
}


void Rooms::styling()
{
    ui->widget->setStyleSheet(
        "QPushButton {"
        "background-color: #2196F3;"
        "color: white;"
        "border: none;"
        "padding: 10px 24px;"
        "font-size: 16px;"
        "border-radius: 12px;"
        "}"

        "QPushButton:hover {"
        "background-color: #1976D2;"
        "}"

        "QPushButton:pressed {"
        "background-color: #1565C0;"
        "}"

        "QPushButton:disabled {"
        "background-color: #d3d3d3;"
        "color: #a9a9a9;"
        "}"

        "QLineEdit {"
        "background-color: #2E2E2E;"
        "color: white;"
        "border: 2px solid #2196F3;"
        "padding: 5px;"
        "border-radius: 5px;"
        "font-size: 14px;"
        "}"

        "QLineEdit:focus {"
        "border: 2px solid #42A5F5;"
        "}"

        "QLineEdit:disabled {"
        "background-color: #444444;"
        "color: #777777;"
        "border: 2px solid #666666;"
        "}"

        "QLabel {"
        "color: white;"
        "font-size: 16px;"
        "font-weight: bold;"
        "background-color: transparent;"
        "}"
        );
}
