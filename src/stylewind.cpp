#include "../include/stylewind.h"

StyleWind::StyleWind(Ui::MainWindow *ui, QObject *parent) : QObject(parent), ui(ui)
{

}

StyleWind::~StyleWind() {}

void StyleWind::setStyleWind()
{
    ui->stackedWidget->setStyleSheet(
        "QPushButton {"
        "background-color: #2196F3;"
        "color: white;"
        "border: none;"
        "padding: 10px 24px;"
        "font-size: 16px;"
        "border-radius: 12px;"
        "}"

        "QPushButton:hover {"
        "background-color: #1976D2;"
        "}"

        "QPushButton:pressed {"
        "background-color: #1565C0;"
        "}"

        "QPushButton:disabled {"
        "background-color: #d3d3d3;"
        "color: #a9a9a9;"
        "}"

        "QLineEdit {"
        "background-color: #2E2E2E;"
        "color: white;"
        "border: 2px solid #2196F3;"
        "padding: 5px;"
        "border-radius: 5px;"
        "font-size: 14px;"
        "}"

        "QLineEdit:focus {"
        "border: 2px solid #42A5F5;"
        "}"

        "QLineEdit:disabled {"
        "background-color: #444444;"
        "color: #777777;"
        "border: 2px solid #666666;"
        "}"

        "QLabel {"
        "color: white;"
        "font-size: 16px;"
        "font-weight: bold;"
        "background-color: transparent;"
        "}"

        "QCheckBox {"
        "    color: white;"
        "    font-size: 14px;"
        "}"

        "QCheckBox:checked {"
        "    color: #2196F3;"
        "}"

        "QCheckBox::indicator {"
        "    width: 20px;"
        "    height: 20px;"
        "    border: 2px solid #2196F3;"
        "    border-radius: 5px;"
        "    background-color: #333333;"
        "}"

        "QCheckBox::indicator:checked {"
        "    background-color: #2196F3;"
        "    border: 2px solid #1E88E5;"
        "    border-radius: 5px;"
        "    position: relative;"
        "}"

        "QCheckBox::indicator:unchecked {"
        "    background-color: #333333;"
        "    border: 2px solid #555555;"
        "}"

        "QCheckBox:hover {"
        "    border: 2px solid #42A5F5;"
        "}"

        "QCheckBox:disabled {"
        "    color: #777777;"
        "    border: 2px solid #555555;"
        "}"


        "QStackedWidget {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #2c3e50, stop:0.5 #34495e, stop:1 #1c2833);"
        "color: white;"
        "}"

        "QTableView, QTableWidget {"
        "background-color: #2c3e50;"
        "color: white;"
        "border: 1px solid #2196F3;"
        "font-size: 14px;"
        "selection-background-color: #1976D2;"
        "selection-color: white;"
        "gridline-color: #34495e;"
        "}"

        "QTableView::item:hover, QTableWidget::item:hover {"
        "background-color: #42A5F5;"
        "color: white;"
        "}"

        "QTableView::item:selected, QTableWidget::item:selected {"
        "background-color: #1976D2;"
        "color: white;"
        "}"

        "QHeaderView::section {"
        "background-color: #34495e;"
        "color: white;"
        "padding: 5px;"
        "border: none;"
        "}"

        "QSlider::groove:horizontal {"
        "background: #34495e;"
        "border-radius: 5px;"
        "height: 8px;"
        "}"

        "QSlider::handle:horizontal {"
        "background: #2196F3;"
        "border: 2px solid #2c3e50;"
        "width: 16px;"
        "height: 16px;"
        "border-radius: 8px;"
        "margin: -4px 0px;"
        "}"

        "QSlider::handle:horizontal:hover {"
        "background: #42A5F5;"
        "}"

        "QSlider::sub-page:horizontal {"
        "background: #42A5F5;"
        "border-radius: 5px;"
        "}"

        "QSlider::add-page:horizontal {"
        "background: #34495e;"
        "border-radius: 5px;"
        "}"

        "#label_autoriz, #label_registr_2 {"
        "color: white;"
        "font-size: 25px;"
        "font-weight: bold;"
        "background-color: transparent;"
        "}"
        );
}
