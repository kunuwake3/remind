#include "errbase.h"
#include "ui_errbase.h"
#include <QMessageBox>
#include <QDir>
#include <QSqlDatabase>
#include <QFileDialog>
#include <QtSql>
#include "main_m1.h"


errbase::errbase(QWidget *parent) : QDialog(parent), ui(new Ui::errbase) {
    ui->setupUi(this);
    ui->label_3->setText(file_db);
    ui->lineEdit->setText(file_db);
}

errbase::~errbase()
{
    delete ui;
}

void errbase::on_pushButton_clicked() {
    is_base_ok = 99;
    close();
}

void errbase::on_pushButton_2_clicked() {
    // Создание новой базы данных

    QString pass_new = ui->passw1->text();
    QString newbase1 = ui->lineEdit->text();

    if (newbase1.length() < 3) {
        QMessageBox::warning(this, "Warning","Very short file name. Requires at least 3 characters.");
        return;
    }

    if (newbase1.right(3) != ".db") {
        file_db = newbase1+".db";
    } else {
        file_db = newbase1;
    }

    if (QFile::exists(file_db)) {
        QMessageBox::warning(this, "Warning","File is exists with name "+file_db+". Please type another name.");
        return;
    }

    QFile file("memz.cfg");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(file_db.toUtf8().data());
        file.close();
    }

    if (pass_new.length() < 3) {
        QMessageBox::warning(this, "Warning","Very short password. Requires at least 3 characters.");
        return;
    }

    passw_x = pass_new+key2;
    getkeypass(pass_new+key2);

    if (key_x == 0) {
        QMessageBox::warning(this, "Warning","Error create new password. Please change password");
        return;
    }

    if (! db1.isValid()) {
        db1 = QSqlDatabase::addDatabase("QSQLITE");
    }
    db1.setDatabaseName(file_db);
    db1.open();

    query = QSqlQuery(db1);
    QString dt11 = "CREATE TABLE `DATA`( `id` Integer NOT NULL PRIMARY KEY AUTOINCREMENT, `ip` Char(80) NOT NULL, `country` Char(50) NOT NULL, `login` Char(200) DEFAULT NULL, `pass1` Char(200) NOT NULL DEFAULT '', `date1` DateTime NOT NULL, `date2` DateTime NOT NULL, `price` Char(50) NOT NULL DEFAULT '0', `cur` Char(50) NOT NULL, `url1` Integer NOT NULL DEFAULT 0, `login2` Char(200) NOT NULL DEFAULT '', `passw2` Char(200) NOT NULL DEFAULT '', `login3` Char(200) NOT NULL DEFAULT '', `passw3` Char(200) NOT NULL DEFAULT '', `info` Char(450) NOT NULL DEFAULT '');";
    query.exec(dt11);
    dt11 = "CREATE UNIQUE INDEX 'Data_id' ON 'DATA'('id');";
    query.exec(dt11);
    dt11 = "CREATE TABLE `HOST` (`id` INTEGER PRIMARY KEY AUTOINCREMENT, `name` Char(100), `link` Char(120), `info` Char(250));";
    query.exec(dt11);
    dt11 = "CREATE TABLE `COUNTRY`(`id` Integer NOT NULL PRIMARY KEY AUTOINCREMENT, `name` Char(50) NOT NULL);";
    query.exec(dt11);
    dt11 = "CREATE TABLE `CUR`(`id` Integer NOT NULL PRIMARY KEY AUTOINCREMENT, `name` Char(50) NOT NULL);";
    if (query.exec(dt11)) {
        db1.close();
        QMessageBox::information(this, "Information","The Database has been created. Click OK to continue.");
        is_base_ok = 3;
        close();
    } else {
        QMessageBox::information(this, "Information","Database creation error ");
    }
}

void errbase::on_pushButton_3_clicked() {
    // Выбрать базу данных
    QString temp1 = file_db;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("SqLite base (*.db *.db3)"));
    if (fileName != "") {
        temp1 = fileName;
        if (!QFile::exists(temp1)) {
            is_base_ok = 0;
            QMessageBox::warning(this, "Warning","File access error. Check for access rights.");
        } else {
            is_base_ok = 3;
            file_db = temp1;
            QFile file("memz.cfg");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(file_db.toUtf8().data());
                file.close();
            }
            close();
        }
    }
}
