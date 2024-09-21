#include "../include/mainwindow.h"
#include "../ui/ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Музыка");

    autoriz = new Autorization(ui, this);
    main_page = new Main_page(ui, this);
    room = new Rooms(nullptr, main_page);
    room_page = new Room_page(ui, this);

    main_page->connect_to_database();

    QFile file("token.txt");
    if (!file.exists() || file.size() == 0)
    {

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
        ui->tableView_users_online->setVisible(false);
        ui->pushButton_playlist->setVisible(false);
        ui->label_room->setVisible(false);
        ui->label_music->setVisible(false);
        ui->pushButton_exit_room->setVisible(false);
        ui->stackedWidget->setCurrentWidget(ui->page_main);
        main_page->get_info();
    }

    connect(this, SIGNAL(on_pushButton_log_in_3_clicked()), autoriz, SLOT(main_page()));
    connect(this, SIGNAL(on_pushButton_registr_3_clicked()), autoriz, SLOT(switch_to_registr()));
    connect(this, SIGNAL(on_pushButton_registration_2_clicked()), autoriz, SLOT(registration()));
    connect(this, SIGNAL(on_pushButton_create_room_clicked()), room, SLOT(new_room()));
    connect(this, SIGNAL(on_pushButton_connect_to_room_clicked()), room, SLOT(connect_to_existed()));
    connect(this, SIGNAL(on_tableView_rooms_doubleClicked(QModelIndex)), main_page, SLOT(switch_to_room(QModelIndex)));
    connect(this, SIGNAL(on_pushButton_exit_room_clicked()), main_page, SLOT(back_to_main()));
    connect(this, SIGNAL(on_pushButton_exit_clicked()), main_page, SLOT(exit_from_acconunt()));
    connect(this, SIGNAL(on_pushButton_playlist_clicked()), room_page, SLOT(show_playlist()));
    connect(this, SIGNAL(on_pushButton_add_mus_clicked()), room_page, SLOT(add_track()));
    connect(this, SIGNAL(on_pushButton_back_2_clicked()), room_page, SLOT(get_track()));
    connect(this, SIGNAL(on_pushButton_del_clicked()), main_page, SLOT(room_delete()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete autoriz;
    delete room;
    delete main_page;
    delete room_page;
}
