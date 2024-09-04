#include "../include/main_page.h"


Main_page::Main_page(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{

}


Main_page::~Main_page()
{

}


void Main_page::get_info()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url("http://localhost:8000/rooms/my");
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

    QNetworkReply *reply = manager.get(request);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

        QJsonObject mainObject = jsonResponse.object();QJsonArray roomsArray = mainObject["rooms"].toArray();

        foreach (const QJsonValue& roomValue, roomsArray)
        {
            QJsonObject roomObject = roomValue.toObject();
            QString roomName = roomObject["name"].toString();
            QString roomCode = roomObject["code"].toString();

            table_values.append(QPair<QString, QString>(roomName, roomCode));
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
    CustomTableModel* customModel = new CustomTableModel(table_values, this);

    ui->tableView_rooms->setModel(customModel);

    ui->tableView_rooms->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_rooms->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableView_rooms->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
}


void Main_page::switch_to_room(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentWidget(ui->page_room);

    QString title;

    if (index.isValid())
    {
        title = "Команата: " + index.sibling(index.row(), 0).data().toString();
    }

    ui->label_room->setText(title);
}


void Main_page::back_to_main()
{
    ui->stackedWidget->setCurrentWidget(ui->page_main);
}


void Main_page::exit_from_acconunt()
{
    QFile file("token.txt");
    if (!file.exists() || file.size() == 0)
    {
        qDebug() << "File is empty or does not exist. Creating the file...";

        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
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

        ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);
    }
}
