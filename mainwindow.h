#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QSystemTrayIcon>
#include "messageslist.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeEvent(QEvent*);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute();
    void setTrayIconActions();
    void showTrayIcon();
    void on_pushButton_clicked();
    void Timer1();
    void Start_m1();
    void on_read_db_clicked();
    void on_pushButton_2_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_4_clicked();

    void slotCustomMenuRequested(QPoint pos);
    void copycell();
    void copyrow();
    void deleterow();

    void on_lineEdit_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QTimer *tmr;
    QStandardItemModel *model;
    MessagesList *msg_list;

protected:
    void closeEvent( QCloseEvent *event );

};

#endif // MAINWINDOW_H
