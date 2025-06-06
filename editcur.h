#ifndef EDITCUR_H
#define EDITCUR_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
    class EditCur;
}

class EditCur : public QDialog {
    Q_OBJECT

public:
    explicit EditCur(QWidget *parent = 0);
    ~EditCur();

private slots:
    void on_pushButton_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_3_clicked();
    void on_load_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::EditCur *ui;
    QStandardItemModel *model;

};

#endif // EDITCUR_H
