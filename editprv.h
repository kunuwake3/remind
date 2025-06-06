#ifndef EDITPRV_H
#define EDITPRV_H

#include <QDialog>
#include <QStandardItemModel>


namespace Ui {
   class editprv;
}

class editprv : public QDialog
{
    Q_OBJECT

public:
    explicit editprv(QWidget *parent = 0);
    ~editprv();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_edit_btn_clicked();

    void on_pushButton_2_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_cancel_edit_clicked();

    void on_save_btn_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_upd_all_base_clicked();

private:
    Ui::editprv *ui;
    QStandardItemModel *model2;

};

#endif // EDITPRV_H
