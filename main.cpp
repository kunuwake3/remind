#include "mainwindow.h"
#include <QApplication>	
#include <QTimer>
#include "main_m1.h"
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QTimer::singleShot(300, &w, SLOT(Start_m1()));
    return a.exec();
}
