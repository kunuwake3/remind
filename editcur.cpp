#include "editcur.h"
#include "ui_editcur.h"
#include "main_m1.h"
#include <QMessageBox>
#include <simplecrypt.h>

int sel_row_cur = 0;

EditCur::EditCur(QWidget *parent) : QDialog(parent), ui(new Ui::EditCur) {
    ui->setupUi(this);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    EditCur::on_load_clicked();

    QPushButton *button = ui->load;
    button->setVisible(false);
}

EditCur::~EditCur() {
    delete ui;
}

void EditCur::on_pushButton_clicked(){
    close();
}

void EditCur::on_tableView_clicked(const QModelIndex &index) {
    int x = index.row();
    QModelIndex index2 = model->index(x,0);
    sel_row_cur = model->data(index2).toInt();
}

void EditCur::on_pushButton_3_clicked(){
    if (sel_row_cur < 1) {
        QMessageBox::information(this, "Warning","Before delete select row");
        return;
    }

    query.prepare("DELETE FROM CUR WHERE `id` = :id;");
    query.bindValue(":id", QString::number(sel_row_cur));
    if (query.exec()) {
        EditCur::on_load_clicked();
        res_edit_c1 = 2;
    } else {
        QMessageBox::information(this, "Warning","An error occurred while deleting the entry.");
    }
    sel_row_cur = 0;
}

void EditCur::on_load_clicked() {
    SimpleCrypt crypto;
    crypto.setKey(key_x);

    model = new QStandardItemModel(1, 2, this);
    ui->tableView->setModel(model);
    QModelIndex index;

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Currency");
    ui->tableView->setColumnWidth(0,35);
    ui->tableView->setColumnWidth(1,260);

    int recs = 0;
    query = QSqlQuery(db1);
    query.exec("SELECT `id` FROM CUR;");
    while (query.next()){
        ++recs;
    }
    query.clear();

    if (recs < 1) return;

    QString info[2][recs];

    int r1 = 0;
    query.exec("SELECT * FROM CUR;");
    while (query.next()){
        info[0][r1] = query.value(0).toString();
        info[1][r1] = crypto.decryptToString(query.value(1).toString());
        ++r1;
    }
    query.clear();

    model->setRowCount(r1);

    for (int y=0; y<r1;++y) {
        for (int x=0;x<2;++x){
            index = model->index(y,x);
            model->setData(index, info[x][y]);
            model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
        }
    }
}

void EditCur::on_pushButton_2_clicked() {
    SimpleCrypt crypto;
    crypto.setKey(key_x);

    QString new_c = ui->lineEdit->text();
    if (new_c.length() < 1) {
        QMessageBox::information(this, "Warning","Too short record. Must be longer than 1 character.");
        return;
    }

    query.prepare("INSERT INTO CUR (`name`) VALUES (:cname)");
    query.bindValue(":cname", crypto.encryptToString(new_c));
    if (query.exec()) {
        ui->lineEdit->setText("");
        EditCur::on_load_clicked();
        res_edit_c1 = 2;
    } else {
        QMessageBox::information(this, "Warning","An error occurred while adding a record.");
    }
}
