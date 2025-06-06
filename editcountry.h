#ifndef EDITCOUNTRY_H
#define EDITCOUNTRY_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
    class EditCountry;
}

class EditCountry : public QDialog {
    Q_OBJECT

public:
    explicit EditCountry(QWidget *parent = 0);
    ~EditCountry();

private slots:
    void on_pushButton_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::EditCountry *ui;
    QStandardItemModel *model;

};

#endif // EDITCOUNTRY_H
