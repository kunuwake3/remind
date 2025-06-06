#ifndef MESSAGESLIST_H
#define MESSAGESLIST_H

#include <QDialog>

namespace Ui {
class MessagesList;
}

class MessagesList : public QDialog {
    Q_OBJECT

public:
    explicit MessagesList(QWidget *parent = 0);
    ~MessagesList();

private slots:
    void on_pushButton_3_clicked();

private:
    Ui::MessagesList *ui;
};

#endif // MESSAGESLIST_H
