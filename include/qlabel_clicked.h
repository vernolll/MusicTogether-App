#ifndef QLABEL_CLICKED_H
#define QLABEL_CLICKED_H

#include <QWidget>
#include <QLabel>
#include <QFileDialog>
#include <QMouseEvent>
#include <QBuffer>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonObject>
#include <QNetworkReply>
#include <QJsonDocument>


#include "../ui/ui_mainwindow.h"


namespace Ui
{
class MainWindow;
}


class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent, Ui::MainWindow *ui);
    void setImage(const QString &imagePath);
    ~ClickedLabel();

signals:
    void clicked(ClickedLabel *click);

public slots:
    void selectImage();

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    Ui::MainWindow *ui;
    QString imagePath;
    QString base64Image;
};

#endif // QLABEL_CLICKED_H
