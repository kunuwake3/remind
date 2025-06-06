#include "editprv.h"
#include "ui_editprv.h"
#include "main_m1.h"
#include <QMessageBox>
#include <QTimer>
#include <simplecrypt.h>

int sel_row_cur2 = 0;

editprv::editprv(QWidget *parent):QDialog(parent), ui(new Ui::editprv){
    ui->setupUi(this);

    ui->lineEdit_1->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton *button = ui->pushButton_3;
    button->setVisible(false);

    QPushButton *button2 = ui->save_btn;
    button2->setVisible(false);

    QPushButton *button3 = ui->cancel_edit;
    button3->setVisible(false);

    QPushButton *button4 = ui->upd_all_base;
    button4->setVisible(false);

    QTimer::singleShot(100, this, SLOT(on_pushButton_3_clicked()));
}

editprv::~editprv(){
    delete ui;
}

void editprv::on_pushButton_clicked(){
    close();
}

void editprv::on_pushButton_3_clicked(){
    model2 = new QStandardItemModel(0, 4, this);
    ui->tableView->setModel(model2);
    QModelIndex index;

    while (model2->rowCount() > 0) model2->removeRow(0);

    int r5=0;
    for (r5=0; r5<4; r5++) {
        model2->setHeaderData(r5, Qt::Horizontal, names2col[r5]);
    }

    for (r5=0; r5<4; r5++) {
        ui->tableView->setColumnWidth(r5,width_2col[r5]);
    }

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    int recs = 0;
    query = QSqlQuery(db1);
    query.exec("SELECT `id` FROM HOST;");
    while (query.next()){
        ++recs;
    }
    query.clear();

    if (recs < 1) return;

    QString info[4][recs];

    int r1 = 0;
    query.exec("SELECT * FROM HOST;");
    while (query.next()){
        info[0][r1] = query.value(0).toString();
        info[1][r1] = crypto.decryptToString(query.value(1).toString());
        info[2][r1] = crypto.decryptToString(query.value(2).toString());
        info[3][r1] = crypto.decryptToString(query.value(3).toString());
        ++r1;
    }
    query.clear();

    model2->setRowCount(r1);

    for (int y=0; y<r1;++y) {
        for (int x=0;x<4;++x){
            index = model2->index(y,x);
            model2->setData(index, info[x][y]);
            model2->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
        }
    }

}

void editprv::on_edit_btn_clicked(){
    // add new record
    SimpleCrypt crypto;
    crypto.setKey(key_x);

    QString name1 = ui->lineEdit_1->text();
    QString link1 = ui->lineEdit_2->text();
    QString info1 = ui->lineEdit_3->text();

    if (name1.length() < 1) {
        QMessageBox::information(this, "Warning","Too short name.");
        return;
    }
    if (link1.length() < 3) {
        QMessageBox::information(this, "Warning","Too short link.");
        return;
    }
    query.clear();

    query.prepare("INSERT INTO HOST (`name`, `link`, `info`) VALUES (:name1, :link1, :info1)");
    query.bindValue(":name1", crypto.encryptToString(name1));
    query.bindValue(":link1", crypto.encryptToString(link1));
    query.bindValue(":info1", crypto.encryptToString(info1));

    if (query.exec()) {
        ui->lineEdit_1->setText("");
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        editprv::on_pushButton_3_clicked();
        res_edit_c1 = 3;
    } else {
        QMessageBox::information(this, "Warning","An error occurred while adding a record.");
    }
    query.clear();
}

void editprv::on_pushButton_2_clicked(){
    if (sel_row_cur2 < 1) {
        QMessageBox::information(this, "Warning","Before delete select row");
        return;
    }
    query.prepare("DELETE FROM HOST WHERE `id` = :id;");
    query.bindValue(":id", QString::number(sel_row_cur2));
    if (query.exec()) {
        editprv::on_pushButton_3_clicked();
        res_edit_c1 = 3;
    } else {
        QMessageBox::information(this, "Warning","An error occurred while deleting the entry.");
    }
    sel_row_cur2 = 0;
}

void editprv::on_tableView_doubleClicked(const QModelIndex &index){
    SimpleCrypt crypto;
    crypto.setKey(key_x);

    int x = index.row();
    QModelIndex index2 = model2->index(x,0);
    int res_edit = model2->data(index2).toInt();
    ui->lineEdit_1->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    if (res_edit > 0) {
        query.clear();
        query.exec("SELECT * FROM HOST WHERE `id` = "+QString::number(res_edit));
        if (query.next()){
           ui->lineEdit_1->setText(crypto.decryptToString(query.value(1).toString()));
           ui->lineEdit_2->setText(crypto.decryptToString(query.value(2).toString()));
           ui->lineEdit_3->setText(crypto.decryptToString(query.value(3).toString()));
        }
        query.clear();
        QPushButton *button1 = ui->edit_btn;
        button1->setVisible(false);
        QPushButton *button2 = ui->save_btn;
        button2->setVisible(true);
        QPushButton *button3 = ui->cancel_edit;
        button3->setVisible(true);
        QPushButton *button4 = ui->pushButton_2;
        button4->setVisible(false);
        nowedithst = res_edit;
    }
}

void editprv::on_cancel_edit_clicked(){
    ui->lineEdit_1->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    QPushButton *button1 = ui->edit_btn;
    button1->setVisible(true);
    QPushButton *button2 = ui->save_btn;
    button2->setVisible(false);
    QPushButton *button3 = ui->cancel_edit;
    button3->setVisible(false);
    QPushButton *button4 = ui->pushButton_2;
    button4->setVisible(true);
    sel_row_cur2 = 0;
    nowedithst = 0;
}

void editprv::on_save_btn_clicked(){
    SimpleCrypt crypto;
    crypto.setKey(key_x);

    if (nowedithst < 1) {
        QMessageBox::information(this, "Warning","Something wrong (no valid id for save);");
        return;
    }

    QString name1 = ui->lineEdit_1->text();
    QString link1 = ui->lineEdit_2->text();
    QString info1 = ui->lineEdit_3->text();

    if (name1.length() < 1) {
        QMessageBox::information(this, "Warning","Too short name.");
        return;
    }
    if (link1.length() < 3) {
        QMessageBox::information(this, "Warning","Too short link.");
        return;
    }

    query.clear();

    query.prepare("UPDATE HOST SET `name` = :name1, `link` = :link1, `info` = :info1 WHERE `id` = :id ;");
    query.bindValue(":name1", crypto.encryptToString(name1));
    query.bindValue(":link1", crypto.encryptToString(link1));
    query.bindValue(":info1", crypto.encryptToString(info1));
    query.bindValue(":id", QString::number(nowedithst));

    if (query.exec()) {
        ui->lineEdit_1->setText("");
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        editprv::on_pushButton_3_clicked();
        res_edit_c1 = 3;
        nowedithst = 0;
        sel_row_cur2 = 0;
        editprv::on_cancel_edit_clicked();
    } else {
        QMessageBox::information(this, "Warning","Error save.");
    }
    query.clear();
}

void editprv::on_tableView_clicked(const QModelIndex &index){
    int x = index.row();
    QModelIndex index2 = model2->index(x,0);
    sel_row_cur2 = model2->data(index2).toInt();
}

void editprv::on_upd_all_base_clicked(){
//
}
