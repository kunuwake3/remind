#include "mainwindow.h"
#include <QApplication>	
#include <QTimer>
#include "main_m1.h"
#include <QMessageBox>
#include <QPalette>
#include <QStyleFactory>
#include <QScreen>

int main(int argc, char *argv[]) {

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_SCALE_FACTOR", QByteArray("1.15")); // масштаб на 115%
    
    QApplication a(argc, argv);

    // Включить стиль Fusion (одинаково на всех платформах)
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Создать тёмную палитру
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25)); // подложка у QLineEdit, QTextEdit
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    a.setPalette(darkPalette);

    MainWindow w;
    w.resize(1200, 900); // увеличиваем стартовый размер окна
    w.show();

    QTimer::singleShot(300, &w, SLOT(Start_m1()));
    return a.exec();
}

