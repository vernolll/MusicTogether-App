#include "../include/mainwindow.h"
#include "../ui/ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Музыка");

    ui->stackedWidget->setCurrentWidget(ui->page_autorization_2);

    autoriz = new Autorization(ui, this);

    connect(this, SIGNAL(on_pushButton_log_in_3_clicked()), autoriz, SLOT(main_page()));
    connect(this, SIGNAL(on_pushButton_registr_3_clicked()), autoriz, SLOT(switch_to_registr()));
    connect(this, SIGNAL(on_pushButton_registration_2_clicked()), autoriz, SLOT(registration()));
}


MainWindow::~MainWindow()
{
    delete ui;
}
