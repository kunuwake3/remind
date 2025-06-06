#ifndef ERRBASE_H
#define ERRBASE_H

#include <QDialog>

namespace Ui {
class errbase;
}

class errbase : public QDialog {
    Q_OBJECT

public:
    explicit errbase(QWidget *parent = 0);
    ~errbase();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::errbase *ui;
};

#endif // ERRBASE_H
