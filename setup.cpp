#include "setup.h"
#include "ui_setup.h"
#include "main_m1.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

Setup::Setup(QWidget *parent) : QDialog(parent), ui(new Ui::Setup) {
    ui->setupUi(this);
    ui->pass->setText("");
    ui->label_5->setText(file_db);
}

Setup::~Setup() {
    delete ui;
}

void Setup::on_pushButton_clicked() {
    is_base_ok = 99;
    close();
}

void Setup::on_pushButton_2_clicked() {
    // select file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("SqLite base (*.db *.db3)"));
    if (fileName != "") {
        file_db = fileName;
        if (!QFile::exists(file_db)) {
            is_base_ok = 0;
            QMessageBox::warning(this, "Warning","File access error. Check for access rights.");
        } else {
            ui->label_5->setText(file_db);
            is_base_ok = 3;
            QFile file("memz.cfg");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(file_db.toUtf8().data());
                file.close();
            }
        }
    }
}

void Setup::on_pushButton_3_clicked(){
    QString pas1 = ui->pass->text();
    if (pas1.length() < 1) {
        QMessageBox::warning(this, "Information","Password is very short.");
        return;
    }
    getkeypass(pas1+key2);
    passw_x = pas1+key2;
    close();
}

void Setup::on_pushButton_4_clicked() {
    is_base_ok = 3;
    close();
}
