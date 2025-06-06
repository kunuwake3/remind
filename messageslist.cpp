#include "messageslist.h"
#include "ui_messageslist.h"
#include "main_m1.h"

MessagesList::MessagesList(QWidget *parent) : QDialog(parent), ui(new Ui::MessagesList) {
    ui->setupUi(this);

    show_msg = 10;
}

MessagesList::~MessagesList() {
    delete ui;
}

void MessagesList::on_pushButton_3_clicked(){
    close();
}
