#include "../include/room_page.h"


Room_page::Room_page(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{
    webSocket = new QWebSocket();

    //mus = new Music(ui, this);

    connect(webSocket, &QWebSocket::connected, this, &Room_page::onConnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &Room_page::onTextMessageReceived);
    connect(webSocket, &QWebSocket::disconnected, this, &Room_page::onDisconnected);
    connect(webSocket, &QWebSocket::errorOccurred, this, &Room_page::onError);
    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &Room_page::binaryReceived);
}


int Room_page::room_id = 0;
QSqlTableModel *Room_page::model = nullptr;
QWebSocket *Room_page::webSocket = nullptr;


Room_page::~Room_page()
{
    //delete mus;
}


void Room_page::binaryReceived(const QByteArray message)
{
    qDebug() << message;
}


void Room_page::draw_table_users(int current_room, Ui::MainWindow *ui)
{
    room_id = current_room;
    //mus->get_room_id(room_id);
    Music::get_room_id(room_id);
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


void Room_page::get_track()
{
    QJsonObject jsonMessage;
    jsonMessage["type"] = 4;
    jsonMessage["data"] = 6; // id трека

    if(webSocket->isValid() && webSocket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonDocument jsonDoc(jsonMessage);
        QString jsonString = QString(jsonDoc.toJson());

        webSocket->sendTextMessage(jsonString);
    }
    else
    {
        qDebug() << webSocket->isValid();
        qDebug() << webSocket->state();
        qDebug() << "WebSocket is not in a valid or connected state. Failed to send message.";
    }
    ui->stackedWidget->setCurrentWidget(ui->page_room);
}
