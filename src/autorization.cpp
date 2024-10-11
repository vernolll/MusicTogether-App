#include "../include/autorization.h"


Autorization::Autorization(Ui::MainWindow *ui, QObject *parent, ClickedLabel *lbl) :
    QObject(parent),
    ui(ui),
    main_page1(ui),
    lbl(lbl)
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
        if(isPasswordStrong(password))
        {
            if(login.size() >= 8)
            {
                QNetworkAccessManager manager;
                QEventLoop loop;

                connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

                QUrl url("http://91.103.140.61/users");
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
                    qDebug() << reply->readAll();
                }

                reply->deleteLater();
            }
            else
            {
                QMessageBox::warning(nullptr, "Ненадежный логин", "Логин должен состоять не менее, чем из 5 символов.");
            }
        }
        else
        {
            QMessageBox::warning(nullptr, "Ненадежный пароль", "Пароль должен содержать не менее 5 символов и состоять из прописных и строчных букв, цифр и специальных символов.", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "Ошибка", "Заполните все поля правильно.");
    }
}


void Autorization::get_me()
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url("http://91.103.140.61/users/me");
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

        QJsonObject object = jsonResponse.object();
        QString username = object["username"].toString();
        QString avatar_path = "http://91.103.140.61/" + object["avatar"].toString();

        qDebug() << avatar_path;
        lbl->setImage(avatar_path);
        ui->label_username->setText(username);
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы.");
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 500)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Внутренняя ошибка сервера");
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }
    reply->deleteLater();
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

        QUrl url("http://91.103.140.61/users/login");
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
                get_me();
                ui->stackedWidget->setCurrentWidget(ui->page_main);
                main_page1.get_info();
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
    if (password.length() < 5)
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


void Autorization::back_to_autoriz()
{
    ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);
}
