#include "../include/rooms.h"
#include "../ui/ui_rooms.h"


Rooms::Rooms(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Rooms)
    , mainPage(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Музыка");

    //mainPage = new Main_page(new Ui::MainWindow, nullptr);

    connect(this, SIGNAL(on_pushButton_conf_clicked()), this, SLOT(creating_room()));
    connect(this, &Rooms::callGetInfo, &mainPage, &Main_page::get_info);
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

            QUrl url("http://localhost:8000/rooms");
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

                //mainPage.get_info();
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

    QUrl url("http://localhost:8000/rooms/enter");
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

        //Main_page::get_info(mainWindowUi);
        //mainPage.get_info();
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
