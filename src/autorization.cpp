#include "../include/autorization.h"
#include "../ui/ui_mainwindow.h"


Autorization::Autorization(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{

}


Autorization::~Autorization()
{

}


void Autorization::switch_to_registr()
{
    ui->stackedWidget->setCurrentWidget(ui->page_registr_2);
}


void Autorization::registration()
{
    QString login = ui->lineEdit_username_reg_2->text();
    QString password = ui->lineEdit_password_reg_2->text();

    if(ui->checkBox_accept_2->isChecked() && !password.isEmpty() && !login.isEmpty())
    {
        QNetworkAccessManager manager;
        QEventLoop loop;

        connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QUrl url("http://localhost:8000/users");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject jsonObject;
        jsonObject["username"] = login;
        jsonObject["password"] = password;
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonData = jsonDoc.toJson();

        QNetworkReply *reply = manager.post(request, jsonData);

        loop.exec();

        if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 201)
        {
            ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);
        }
        else
        {
            qDebug() << "Error: " << reply->errorString();
            QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        }

        reply->deleteLater();
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Заполните все поля правильно.");
    }
}


void Autorization::main_page()
{
    QString login = ui->lineEdit_username_3->text();
    QString password = ui->lineEdit_password_3->text();

    if(!login.isEmpty() && !password.isEmpty())
    {
        QNetworkAccessManager manager;
        QEventLoop loop;

        connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QUrl url("http://localhost:8000/users/login");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject jsonObject;
        jsonObject["username"] = login;
        jsonObject["password"] = password;
        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonData = jsonDoc.toJson();

        QNetworkReply *reply = manager.post(request, jsonData);

        loop.exec();

        if(reply->error() == QNetworkReply::NoError)
        {
            QByteArray responseData = reply->readAll();
            QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
            QString token = responseJson.object().value("access_token").toString();

            if(!token.isEmpty())
            {
                ui->stackedWidget->setCurrentWidget(ui->page_main);
            }
            else
            {
                QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при авторизации.");
            }
        }
        else
        {
            qDebug() << "Network Error: " << reply->errorString();
            QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Заполните все поля правильно.");
    }
}
