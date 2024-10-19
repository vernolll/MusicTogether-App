#include "../include/mainwindow.h"
#include "../ui/ui_mainwindow.h"


QString server_path = "91.103.140.61";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Музыка");

    clickedLabel = new ClickedLabel(ui->label_avatar->parentWidget(), ui);
    autoriz = new Autorization(ui, this, clickedLabel);
    main_page = new Main_page(ui, this);
    room = new Rooms(nullptr, main_page);
    room_page = new Room_page(ui, this);

    ui->verticalLayout_8->replaceWidget(ui->label_avatar, clickedLabel);
    ui->label_avatar = clickedLabel;

    connect(clickedLabel, &ClickedLabel::clicked, clickedLabel, &ClickedLabel::selectImage);

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

        ui->tableWidget_users->setVisible(false);
        ui->pushButton_playlist->setVisible(false);
        ui->label_room->setVisible(false);
        ui->label_music->setVisible(false);
        ui->pushButton_exit_room->setVisible(false);
        ui->pushButton_synchron->setVisible(false);
    }
    else
    {
        ui->tableWidget_users->setVisible(false);
        ui->pushButton_playlist->setVisible(false);
        ui->label_room->setVisible(false);
        ui->label_music->setVisible(false);
        ui->pushButton_exit_room->setVisible(false);
        ui->pushButton_synchron->setVisible(false);
        autoriz->get_me();
        ui->stackedWidget->setCurrentWidget(ui->page_main);
        main_page->get_info();
    }

    ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(this, SIGNAL(on_pushButton_log_in_3_clicked()), autoriz, SLOT(main_page()));
    connect(this, SIGNAL(on_pushButton_registr_3_clicked()), autoriz, SLOT(switch_to_registr()));
    connect(this, SIGNAL(on_pushButton_registration_2_clicked()), autoriz, SLOT(registration()));
    connect(this, SIGNAL(on_pushButton_create_room_clicked()), room, SLOT(new_room()));
    connect(this, SIGNAL(on_pushButton_connect_to_room_clicked()), room, SLOT(connect_to_existed()));
    connect(this, SIGNAL(on_tableView_rooms_doubleClicked(QModelIndex)), main_page, SLOT(switch_to_room(QModelIndex)));
    connect(this, SIGNAL(on_pushButton_exit_room_clicked()), room_page, SLOT(leaving_room()));
    connect(this, SIGNAL(on_pushButton_exit_clicked()), main_page, SLOT(exit_from_acconunt()));
    connect(this, SIGNAL(on_pushButton_playlist_clicked()), room_page, SLOT(show_playlist()));
    connect(this, SIGNAL(on_pushButton_add_mus_clicked()), room_page, SLOT(add_track()));
    connect(this, SIGNAL(on_pushButton_del_clicked()), main_page, SLOT(room_delete()));
    connect(this, SIGNAL(on_pushButton_back_2_clicked()), main_page, SLOT(back_to_main()));
    connect(this, SIGNAL(on_pushButton_synchron_clicked()), room_page, SLOT(send_synchron()));
    connect(this, SIGNAL(on_horizontalSlider_volume_sliderMoved(int)), room_page, SLOT(setting_volume(int)));
    connect(this, SIGNAL(on_horizontalSlider_music_sliderMoved(int)), room_page, SLOT(send_rewind(int)));
    connect(this, SIGNAL(on_pushButton_back_clicked()), autoriz, SLOT(back_to_autoriz()));
    connect(this, SIGNAL(on_pushButton_play_clicked()), room_page, SLOT(playbutton()));
    connect(this, SIGNAL(on_pushButton_search_clicked()), room_page, SLOT(searching()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete autoriz;
    delete room;
    delete main_page;
    delete room_page;
    delete clickedLabel;
}

