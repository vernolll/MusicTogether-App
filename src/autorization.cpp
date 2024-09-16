#include "../include/autorization.h"


Autorization::Autorization(Ui::MainWindow *ui, QObject *parent) :
    QObject(parent),
    ui(ui)
{
    //main_page1 = new Main_page(ui, this);
}


Autorization::~Autorization()
{
    //delete main_page1;
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
        if(isPasswordStrong(password))
        {
            if(login.size() >= 8)
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
                else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 409)
                {
                    QMessageBox::warning(nullptr, "Ошибка", "Такой пользователь уже существует");
                }
                else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
                {
                    QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера");
                }
                else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400)
                {
                    QMessageBox::warning(nullptr, "Ошибка", "Плохой запрос.");
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
                QMessageBox::warning(nullptr, "Ненадежный логин", "Логин должен состоять не менее, чем из 8 символов.");
            }
        }
        else
        {
            QMessageBox::warning(nullptr, "Ненадежный пароль", "Пароль должен содержать не менее 8 символов и состоять из прописных и строчных букв, цифр и специальных символов.", QMessageBox::Ok);
        }
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

            QFile file("token.txt");
            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << token;
                file.close();
            }
            else
            {
                qDebug() << "Failed to open file for writing.";
            }

            if(!token.isEmpty())
            {
                ui->stackedWidget->setCurrentWidget(ui->page_main);
                //Main_page::connect_to_database();
                //Main_page::get_info();
                //main_page1->connect_to_database();
                //main_page1->get_info();
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
         reply->deleteLater();
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Заполните все поля правильно.");
    }
}


bool Autorization::isPasswordStrong(const QString &password)
{
    if (password.length() < 8)
    {
        return false;
    }

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (const QChar &c : password)
    {
        if (c.isUpper())
        {
            hasUpper = true;
        }
        else if (c.isLower())
        {
            hasLower = true;
        }
        else if (c.isDigit())
        {
            hasDigit = true;
        }
        else if (!c.isSpace() && !c.isLetterOrNumber())
        {
            hasSpecial = true;
        }
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

