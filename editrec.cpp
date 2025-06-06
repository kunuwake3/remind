#include "editrec.h"
#include "ui_editrec.h"
#include "editcountry.h"
#include "editcur.h"
#include "editprv.h"
#include "main_m1.h"
#include <QMessageBox>
#include <QRegExpValidator>
#include <QKeyEvent>
#include <QClipboard>
#include <simplecrypt.h>

int del_ip = 0;
QString last_ip = "";


EditRec::EditRec(QWidget *parent) : QDialog(parent), ui(new Ui::EditRec) {
    ui->setupUi(this);

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    QString bb1 = "";
    int bb2 = 0;

    if (sets3 != "T") {
        QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
        QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
        QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, NULL);
        if (odd_ip_input > 0) {
            ui->ip_addr->setValidator(ipValidator);
        }
    } else {
        ui->label->setText("IP/label/text");
    }

    QDate date = QDate::currentDate();
    QDate m11 = date.addMonths(1);
    ui->dateEdit->setDate(date);
    ui->dateEdit_2->setDate(m11);

    res_edit_c1 = 0;
    EditRec::on_pushButton_4_clicked();

    QPushButton *button = ui->pushButton_4;
    button->setVisible(false);

    if (res_edit >0) {

        QString title_new = "Edit record #"+QString::number(res_edit);
        QWidget::setWindowTitle(title_new);

        QString r0 = "";
        QDate blank_date(1999, 1, 1);
        QDate dt1;

        query = QSqlQuery(db1);
        query.exec("SELECT * FROM DATA WHERE `id` = "+QString::number(res_edit));
        if (query.next()){
           ui->ip_addr->setText(crypto.decryptToString(query.value(1).toString()));
           ui->country->setCurrentText(crypto.decryptToString(query.value(2).toString()));
           ui->login1->setText(crypto.decryptToString(query.value(3).toString()));
           ui->pass1->setText(crypto.decryptToString(query.value(4).toString()));
           r0 = query.value(5).toString();
           if (r0.length()< 6) {
               ui->dateEdit->setDate(blank_date);
           } else {
               dt1 = query.value(5).toDate();
               ui->dateEdit->setDate(dt1);
           }
           r0 = query.value(6).toString();
           if (r0.length()< 6) {
               ui->dateEdit_2->setDate(blank_date);
           } else {
               dt1 = query.value(6).toDate();
               ui->dateEdit_2->setDate(dt1);
           }
           ui->price->setText(crypto.decryptToString(query.value(7).toString()));
           ui->cur->setCurrentText(crypto.decryptToString(query.value(8).toString()));

           bb1 = query.value(9).toString();
           bb2 = ui->hosts->findData(bb1);
           if (bb2 > -1) {
             ui->hosts->setCurrentIndex(bb2);
           }

           ui->login_host->setText(crypto.decryptToString(query.value(10).toString()));
           ui->pass_host->setText(crypto.decryptToString(query.value(11).toString()));
           ui->login_mail->setText(crypto.decryptToString(query.value(12).toString()));
           ui->pass_mail->setText(crypto.decryptToString(query.value(13).toString()));
           ui->textedit->setPlainText(crypto.decryptToString(query.value(14).toString()));
        }
        query.clear();

        res_edit = 0;
    } else {
        QString title_new = "Add new record";
        QWidget::setWindowTitle(title_new);
        QPushButton *button1 = ui->pushButton_3;
        button1->setVisible(false);
    }


    // test button
    QPushButton *button1 = ui->pushButton_6;
    button1->setVisible(false);

    // установить курсор на 0
    //ui->ip_addr->setCursorPosition(0);
    edited = 0;
}

EditRec::~EditRec() {
    delete ui;
}

void EditRec::on_eClose_clicked(){
    close();
}

void EditRec::reject(){
    if (edited == 0) {
        close();
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Close unsaved", "Do you want close without SAVE?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        edited = 0;
        close();
    }
}

void EditRec::closeEvent(QCloseEvent *res_e){
    if (edited == 0) {
        res_e->accept();
    } else {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close unsaved", "Do you want close without SAVE?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            res_e->ignore();
        }
    }
}

void EditRec::on_pushButton_clicked() {
    res_edit_c1 = 10;
    EditCountry win4;
    win4.setModal(true);
    win4.exec();
    if (res_edit_c1 != 10) EditRec::on_pushButton_4_clicked();
}

void EditRec::on_pushButton_2_clicked(){
    res_edit_c1 = 10;
    EditCur win5;
    win5.setModal(true);
    win5.exec();
    if (res_edit_c1 != 10) EditRec::on_pushButton_4_clicked();
}

void EditRec::on_pushButton_5_clicked(){
    res_edit_c1 = 10;
    editprv win6;
    win6.setModal(true);
    win6.exec();
    if (res_edit_c1 != 10) EditRec::on_pushButton_4_clicked();
}

void EditRec::on_textedit_textChanged() {
    edited = 10;
    int nums = ui->textedit->toPlainText().length();
    QFont fnt = this->font();
    if (nums > 254) {
        if (black_theme == 0) ui->textedit->setStyleSheet("QPlainTextEdit {background-color: #FFD0D0;}");
        if (black_theme == 1) ui->textedit->setStyleSheet("QPlainTextEdit {background-color: #D09090; color: black;}");
        ui->textedit->setFont(fnt);
        ui->textedit->setBackgroundVisible(true);
    } else {
        if (ui->textedit->backgroundVisible()) {
            if (black_theme == 0) ui->textedit->setStyleSheet("QPlainTextEdit {}");
            if (black_theme == 1) ui->textedit->setStyleSheet("QPlainTextEdit {background-color: #404040; color:#F0F0F0}");
            ui->textedit->setFont(fnt);
            ui->textedit->setBackgroundVisible(false);
        }
    }
}

void EditRec::on_OkBtn_clicked(){
    // Save or create

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    QString n2 = "";
    QString n3 = "";

    QString a1 = this->windowTitle();
    QString r0 = "";
    if (a1.left(4) == "Edit") {
        // Edit record -> Save
        a1 = a1.mid(13,-1);

        r0 = ui->ip_addr->text();
        if (r0.length() < 3) {
            QMessageBox::information(this, "Information","Too short IP address");
            return;
        }
        r0 = ui->textedit->toPlainText();
        if (r0.length() > 2550) {
            QMessageBox::information(this, "Information","Too long text in information box. Limit 2550 characters.");
            return;
        }

        if (ui->dateEdit->date() > ui->dateEdit_2->date()) {
            QMessageBox::information(this, "Information","Check the dates are entered correctly.");
            return;
        }

        query.prepare("UPDATE DATA SET `ip` = :wip, `country` = :wcountry, `login` = :wlogin, `pass1` = :wpass1, `date1` = :wdate1, `date2` = :wdate2, `price` = :wprice, `cur` = :wcur, `url1` = :wurl1, `login2` = :wlogin2, `passw2` = :wpassw2, `login3` = :wlogin3, `passw3` = :wpassw3, `info` = :winfo WHERE `id` = :id ;");
        query.bindValue(":wip", crypto.encryptToString(ui->ip_addr->text()));
        query.bindValue(":wcountry", crypto.encryptToString(ui->country->currentText()));
        query.bindValue(":wlogin", crypto.encryptToString(ui->login1->text()));
        query.bindValue(":wpass1", crypto.encryptToString(ui->pass1->text()));
        query.bindValue(":wdate1", ui->dateEdit->date().toString("yyyy-MM-dd"));
        query.bindValue(":wdate2", ui->dateEdit_2->date().toString("yyyy-MM-dd"));
        query.bindValue(":wprice", crypto.encryptToString(ui->price->text()));
        query.bindValue(":wcur", crypto.encryptToString(ui->cur->currentText()));

        n2 = ui->hosts->currentData().toString();
        if (n2 == "") n2 = "0";
        query.bindValue(":wurl1", n2);

        query.bindValue(":wlogin2", crypto.encryptToString(ui->login_host->text()));
        query.bindValue(":wpassw2", crypto.encryptToString(ui->pass_host->text()));
        query.bindValue(":wlogin3", crypto.encryptToString(ui->login_mail->text()));
        query.bindValue(":wpassw3", crypto.encryptToString(ui->pass_mail->text()));
        query.bindValue(":winfo", crypto.encryptToString(ui->textedit->toPlainText()));
        query.bindValue(":id", a1);

        if (!query.exec()) {
            QMessageBox::information(this, "Warning","Error save.");
        } else {
            res_edit = 5;
            edited = 0;
            close();
        }

    } else {
        // New record -> Create
        r0 = ui->ip_addr->text();
        if (r0.length() < 3) {
            QMessageBox::information(this, "Information","Too short IP address");
            return;
        }
        r0 = ui->textedit->toPlainText();
        if (r0.length() > 255) {
            QMessageBox::information(this, "Information","Too long text in information box. Limit 255 characters.");
            return;
        }

        if (ui->dateEdit->date() > ui->dateEdit_2->date()) {
            QMessageBox::information(this, "Information","Check the dates are entered correctly.");
            return;
        }

        query.prepare("INSERT INTO DATA (`ip`, `country`, `login`, `pass1`, `date1`, `date2`, `price`, `cur`, `url1`, `login2`, `passw2`, `login3`, `passw3`, `info`) VALUES (:wip, :wcountry, :wlogin, :wpass1, :wdate1, :wdate2, :wprice, :wcur, :wurl1, :wlogin2, :wpassw2, :wlogin3, :wpassw3, :winfo);");
        query.bindValue(":wip", crypto.encryptToString(ui->ip_addr->text()));
        query.bindValue(":wcountry", crypto.encryptToString(ui->country->currentText()));
        query.bindValue(":wlogin", crypto.encryptToString(ui->login1->text()));
        query.bindValue(":wpass1", crypto.encryptToString(ui->pass1->text()));
        query.bindValue(":wdate1", ui->dateEdit->date().toString("yyyy-MM-dd"));
        query.bindValue(":wdate2", ui->dateEdit_2->date().toString("yyyy-MM-dd"));
        query.bindValue(":wprice", crypto.encryptToString(ui->price->text()));
        query.bindValue(":wcur", crypto.encryptToString(ui->cur->currentText()));

        n2 = ui->hosts->currentData().toString();
        if (n2 == "") n2 = "0";
        query.bindValue(":wurl1", n2);

        query.bindValue(":wlogin2", crypto.encryptToString(ui->login_host->text()));
        query.bindValue(":wpassw2", crypto.encryptToString(ui->pass_host->text()));
        query.bindValue(":wlogin3", crypto.encryptToString(ui->login_mail->text()));
        query.bindValue(":wpassw3", crypto.encryptToString(ui->pass_mail->text()));
        query.bindValue(":winfo", crypto.encryptToString(ui->textedit->toPlainText()));

        if (query.exec()) {
            res_edit = 5;
            edited = 0;
            close();
        } else {
            QMessageBox::information(this, "Warning","Error save.");
        }
    }
}

void EditRec::on_pushButton_4_clicked(){

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    if ((res_edit_c1 == 1)||(res_edit_c1 == 0)){
        ui->country->clear();
        ui->country->addItem("");

        query.exec("SELECT * FROM COUNTRY;");
        while (query.next()){
            ui->country->addItem(crypto.decryptToString(query.value(1).toString()));
        }
        query.clear();
    }

    if ((res_edit_c1 == 2)||(res_edit_c1 == 0)){
        ui->cur->clear();
        ui->cur->addItem("");
        query.exec("SELECT * FROM CUR;");
        while (query.next()){
            ui->cur->addItem(crypto.decryptToString(query.value(1).toString()));
        }
        query.clear();
    }

    if ((res_edit_c1 == 3)||(res_edit_c1 == 0)){
        ui->hosts->clear();
        ui->hosts->addItem("");
        query.exec("SELECT * FROM HOST;");
        while (query.next()){
            ui->hosts->addItem(crypto.decryptToString(query.value(1).toString()),query.value(0).toString());

        }
        query.clear();
    }


    res_edit_c1 = 0;
}

void EditRec::on_pushButton_3_clicked(){
    QString a1 = this->windowTitle();
    if (a1.left(4) != "Edit") {
        //return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm delete", "Do you want to delete this entry from the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        QString a1 = this->windowTitle();
        if (a1.left(4) == "Edit") {
            a1 = a1.mid(13,-1);
            query = QSqlQuery(db1);
            query.prepare("DELETE FROM DATA WHERE `id` = :id;");
            query.bindValue(":id", a1);
            if (query.exec()) {
            } else {
                QMessageBox::information(this, "Warning","An error occurred while deleting the entry.");
                return;
            }
            res_edit = 5;
            close();
        } else {
            QMessageBox::information(this, "Warning","Something wrong. An error occurred while deleting the entry.");
        }
    }
}

void EditRec::on_ip_addr_textChanged(){
    edited = 1;
}

void EditRec::on_login1_textChanged(){ edited = 2; }
void EditRec::on_pass1_textChanged(){ edited = 3; }
void EditRec::on_price_textChanged(){ edited = 4; }

// void EditRec::on_url_host_textChanged(){ edited = 5; }


void EditRec::on_login_host_textChanged(){ edited = 6; }
void EditRec::on_pass_host_textChanged(){ edited = 7; }
void EditRec::on_login_mail_textChanged(){ edited = 8; }
void EditRec::on_pass_mail_textChanged(){ edited = 9; }
void EditRec::on_dateEdit_dateChanged(){ edited = 11; }
void EditRec::on_dateEdit_2_dateChanged(){ edited = 12; }
void EditRec::on_country_currentIndexChanged(const QString &arg1){ edited = 13; }
void EditRec::on_cur_currentIndexChanged(const QString &arg1){ edited = 14; }

void EditRec::on_ip_addr_textEdited(const QString &arg1){

    del_ip = 0;
    if (odd_ip_input < 3) {
        return;
    }

    QString p0 = ui->ip_addr->text();
    if (p0.count(".") >2) return;
    if (p0.right(1) == ".") return;

    QString p1 = p0;
    QString p2 = p0;
    int last_dot = p0.lastIndexOf(".");
    if (last_dot > -1) {
        p0 = p0.right((p0.length()-last_dot)-1);
        //if (p0.indexOf(".") > -1) p0 = p0.right(2);
    }

    if (p0.length() > 1) {
       int e0 = p0.toInt();
       if (e0 > 25) {
           p1 = p1 + ".";
           ui->ip_addr->setText(p1);
           if (p1 != p2) del_ip = 1;
       }
    }

}


void EditRec::keyReleaseEvent(QKeyEvent* event){

    if (odd_ip_input == 2) {
        switch(event->key()) {
        case Qt::Key_Comma:{
            QString p0 = ui->ip_addr->text();
            if (p0.right(1) != ".") {
                if (p0.count(".") <3) ui->ip_addr->setText(p0+".");
            }
        }
        }
        return;
    }

    if (odd_ip_input < 3) {
        last_ip = ui->ip_addr->text();
        return;
    }

    QString p0 = ui->ip_addr->text();
    switch(event->key()) {
    case Qt::Key_Backspace:{
        if (p0.right(1) == ".") {
            p0 = p0.left(p0.length()-1);
            if (del_ip == 1) {
                del_ip = 0;
                ui->ip_addr->setText(p0);
            }
        }
       break;
    }
    case Qt::Key_Comma:{
        if ((p0.right(1)!=".")&(p0.count(".")<3)) ui->ip_addr->setText(p0+".");
        break;
    }
    case Qt::Key_0:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".0");
        break;
    }
    case Qt::Key_1:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".1");
        break;
    }
    case Qt::Key_2:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".2");
        break;
    }
    case Qt::Key_3:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".3");
        break;
    }
    case Qt::Key_4:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".4");
        break;
    }
    case Qt::Key_5:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".5");
        break;
    }
    case Qt::Key_6:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".6");
        break;
    }
    case Qt::Key_7:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".7");
        break;
    }
    case Qt::Key_8:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".8");
        break;
    }
    case Qt::Key_9:{
        if ((p0.right(1)!=".")&(del_ip==0)&(last_ip==p0)&(p0.count(".")<3)) ui->ip_addr->setText(p0+".9");
        break;
    }
    }
    last_ip = ui->ip_addr->text();
}

void EditRec::on_pushButton_7_clicked(){
    ui->login1->setEchoMode(QLineEdit::Normal);
}

void EditRec::on_pushButton_8_clicked(){
    ui->pass1->setEchoMode(QLineEdit::Normal);
}

void EditRec::on_pushButton_9_clicked(){
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->login1->text());
}

void EditRec::on_pushButton_10_clicked(){
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->pass1->text());
}

void EditRec::on_pushButton_11_clicked(){
    ui->pass_host->setEchoMode(QLineEdit::Normal);
}

void EditRec::on_pushButton_12_clicked(){
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->pass_host->text());
}

void EditRec::on_pushButton_13_clicked(){
    ui->pass_mail->setEchoMode(QLineEdit::Normal);
}

void EditRec::on_pushButton_14_clicked(){
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->pass_mail->text());
}


void EditRec::on_pushButton_6_clicked(){
    // test mode


}

void EditRec::on_pushButton_15_clicked() {
    // Copy LINK

    QString a1 = ui->hosts->currentData().toString();

    if (a1.length() < 1) {
        QMessageBox::information(this, "Warning","No selected host");
        return;
    }
    if (a1.length() > 7) {
        QMessageBox::information(this, "Warning","Over limits!");
        return;
    }

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    QString link1 = "";

    query.clear();
    query.exec("SELECT * FROM HOST WHERE `id` = "+a1+";");
    if (query.next()){
        link1 = crypto.decryptToString(query.value(2).toString());
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(link1);
    } else {
        QMessageBox::information(this, "Warning","No find: "+a1);
    }
    query.clear();
}
