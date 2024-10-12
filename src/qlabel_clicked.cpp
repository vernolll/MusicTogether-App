#include "../include/qlabel_clicked.h"


ClickedLabel::ClickedLabel(QWidget* parent, Ui::MainWindow *ui)
    : QLabel(parent), ui(ui)
{
}


ClickedLabel::~ClickedLabel() {}

extern QString server_path;

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(this);
    event->accept();
}


void ClickedLabel::selectImage()
{
    imagePath = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg)"));

    if (!imagePath.isEmpty())
    {
        setImage(imagePath);
    }

    QNetworkAccessManager manager;
    QEventLoop loop;

    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QUrl url("http://" + server_path + "/users/avatar");
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
    trackObject["data"] = base64Image;

    QFileInfo fileInfo(imagePath);
    QString fileName = fileInfo.fileName();
    trackObject["filename"] = fileName;

    QJsonDocument trackDoc(trackObject);
    QByteArray postData = trackDoc.toJson();

    QNetworkReply *reply = manager.put(request, postData);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        qDebug() << "new photo";
        return;
    }
    else if(reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 400)
    {
        QMessageBox::warning(nullptr, "Ошибка", "Плохой запрос.");
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
        qDebug() << "Network Error: " << reply->errorString() << "(new_avatar)";
        qDebug() <<  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QMessageBox::warning(nullptr, "Ошибка", "Произошла ошибка при отправке запроса.");
    }

    reply->deleteLater();
}


void ClickedLabel::setImage(const QString &imagePath)
{
    QPixmap image;

    if (imagePath.startsWith("http://") || imagePath.startsWith("https://"))
    {
        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(imagePath)));
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray imageData = reply->readAll();
            image.loadFromData(imageData);
        }
        else
        {
            qDebug() << "Failed to load image from URL:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        reply->deleteLater();
    }
    else
    {
        if (!image.load(imagePath))
        {
            qDebug() << "Failed to load image from local path:" << imagePath;
            return;
        }
    }

    int sideLength = qMin(image.width(), image.height());
    int xOffset = (image.width() - sideLength) / 2;
    int yOffset = (image.height() - sideLength) / 2;

    QPixmap croppedImage = image.copy(xOffset, yOffset, sideLength, sideLength);
    setPixmap(croppedImage.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    update();

    QImage img = croppedImage.toImage();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");

    base64Image = QString::fromLatin1(byteArray.toBase64().data());
}
