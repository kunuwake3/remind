#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main_m1.h"
#include "errbase.h"
#include "editrec.h"
#include "setup.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QSqlDatabase>
#include <QtSql>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>
#include <QProcess>
#include <QClipboard>
#include <QSound>
#include <simplecrypt.h>

int clicksearch = 0;
int hide_after_start = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this -> setTrayIconActions();
    this -> showTrayIcon();

    // таймер старта приложения - проверка наличия базы и прочее
    tmr = new QTimer();
    connect(tmr,SIGNAL(timeout()), this, SLOT(Timer1()));
    tmr->start(3000);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    show_msg = 0;


    QPushButton *button_1 = ui->pushButton_3;
    button_1->setVisible(false);

    QPushButton *button_2 = ui->pushButton;
    button_2->setVisible(false);

    sets3 = "-";
    res_edit_c1 = 0;
    int c1 =  QCoreApplication::arguments().count();
    QString in_com = "";
    if (c1 > 1) {
        for (int b=1; b < c1; ++b) {
            in_com = QCoreApplication::arguments().at(b);
            if (in_com == "-hide") {
                hide_after_start = 1;
            }
            if (in_com == "-textmode") {
                sets3 = "T";
            }
            if ((in_com == "-h")|(in_com == "-help")) {
                QMessageBox::information(this, "Console commands"," -hide | Start Memorize hided\n -testsound | Play test sound at startup \n -h | Show this \n -textmode | Text mode (is no strict verification of IP address)");
            }
            if (in_com == "-testsound") res_edit_c1 = 1;
        }
    }

    if (res_edit_c1 == 1) {
        QSound::play(":/snd/snd1.wav");
        res_edit_c1 = 0;
    }

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (edited != 777) {
        event->ignore();
        hide();
    }
}


void MainWindow::slotCustomMenuRequested(QPoint pos){

    QMenu * menu = new QMenu(this);
    QAction * tab_copy_cell = new QAction("Copy CELL", this);
    QAction * tab_copy_row = new QAction("Copy ROW", this);
    QAction * delete_row = new QAction("Delete this record", this);

    int r0=0;
    int r1=0;
    int r2=pos.x();
    for (r0=0; r0<cols1; r0++) {
        if (r2>0) {
            //r2 = r2 - width_col.at(r0);
            r2 = r2 - width_col[r0];
            r1++;
        }
    }
    sel_cell_x = r1-1;
    sel_cell_y = (pos.y() / 30);

    connect(tab_copy_cell, SIGNAL(triggered()), this, SLOT(copycell()));
    connect(tab_copy_row, SIGNAL(triggered()), this, SLOT(copyrow()));
    connect(delete_row, SIGNAL(triggered()), this, SLOT(deleterow()));

    menu->addAction(tab_copy_cell);
    menu->addAction(tab_copy_row);
    menu->addSeparator();
    menu->addAction(delete_row);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}


void MainWindow::showTrayIcon(){
    trayIcon = new QSystemTrayIcon(this);
    QIcon trayImage(":/img/m1.png");
    trayIcon -> setIcon(trayImage);
    trayIcon -> setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon -> show();
}

void MainWindow::trayActionExecute() {
    if (isVisible()) {
        hide();
    } else {
        showNormal();
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this -> trayActionExecute();
            break;
        default:
            break;
    }
}

void MainWindow::setTrayIconActions() {
    minimizeAction = new QAction("Hide", this);
    restoreAction = new QAction("Restore", this);
    quitAction = new QAction("Quit", this);
    connect (minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect (restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // По такому же принципу можно добавить иконки в меню трея
    //minimizeAction->setIcon(QIcon(":/img/m3.png"));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction (minimizeAction);
    trayIconMenu->addAction (restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction (quitAction);
}

void MainWindow::changeEvent(QEvent *event) {
    QMainWindow::changeEvent(event);
    if (event -> type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            this -> hide();
        }
    }
}

void MainWindow::Timer1() {
    if (is_base_ok == 99) {
        edited = 777;
        close();
    }

    /*
Суть проекта:
Приложение должно напоминать об оплате за 14 дней до назначенной даты конца оплаты.
Предполагается, что приложение будет загружено автоматически со стартом системы и постоянно будет висеть свернутое.
Напоминия происходят звуком, сам звук можешь выбрать сам.

Цикличность напоминаний:
от 14 до 10 дней – 1 раз в день, время напоминания 15.00 по времени ПК
от 9 до 3 дней – 2 раза в день, время напоминаний в 12.00 и в 18.00 по времени ПК
от 2 до 0 дней – 4 раза в день, время напоминаний в 9.00, в 12.00, в 15.00, в 18.00 по времени ПК
Напоминания не закрываются пока они фактически не будут закрыты пользователем (типа уведомлен, ок)
     */


    if (count_rows < 1) {
        return;
    }

    //timer_hidez
    if (QWidget::isHidden()) {
        if (timer_hidez < 42) {
            ++timer_hidez;
        }
        if (timer_hidez == 40) {
            is_blocked_pass = 1;
            setWindowTitle("Memorize | Blocked");
        }
    } else {
        timer_hidez = 0;
    }

    QModelIndex index;
    QModelIndex index2;
    QString r0 = "";
    QString r1 = "";
    QString r2 = "";
    QDate dt1;
    QDate dt2 = QDate::currentDate();
    QDate dt3;
    QTime tm1 = QTime::currentTime();
    QString time_h = tm1.toString("hh:mm");
    int days1 = 0;

    if (time_h == "00:00") show_msg = 0;
    if (time_h == "10:00") show_msg = 0;
    if (time_h == "13:00") show_msg = 0;
    if (time_h == "16:00") show_msg = 0;

    QString notf = "";

    int rows = ui->tableView->model()->rowCount();
    int x;

    for (x=0;x<rows;++x){
        index2 = model->index(x,3); // 6
        r0 = model->data(index2).toString();
        dt1 = QDate::fromString(r0, "dd-MM-yyyy");
        if (dt1 < dt2) {
            model->setData(index2, QBrush(Qt::red), Qt::BackgroundRole);
        } else {
            // 2 days
            dt3 = dt2.addDays(3);
            if (dt1 < dt3) {
                if (black_theme == 0) model->setData(index2, QBrush("#FFFF00"), Qt::BackgroundRole);
                if (black_theme == 1) model->setData(index2, QBrush("#A0A000"), Qt::BackgroundRole);
                if ((time_h == "09:00")|(time_h == "12:00")|(time_h == "15:00")|(time_h == "18:00")) {
                    index2 = model->index(x,1);
                    r0 = model->data(index2).toString();
                    index2 = model->index(x,4);
                    r1 = model->data(index2).toString();
                    days1 = dt2.daysTo(dt1);
                    notf = notf + "Need pay IP:"+r0+" price "+r1+" Low "+ QString::number(days1) +" days \n ";
                }
            } else {
                //
                dt3 = dt2.addDays(10);
                if (dt1 < dt3) {
                    if (black_theme == 0) model->setData(index2, QBrush("#FFFF90"), Qt::BackgroundRole);
                    if (black_theme == 1) model->setData(index2, QBrush("#909030"), Qt::BackgroundRole);

                    if ((time_h == "12:00")|(time_h == "18:00")) {
                        index2 = model->index(x,1);
                        r0 = model->data(index2).toString();
                        index2 = model->index(x,4);
                        r1 = model->data(index2).toString();
                        days1 = dt2.daysTo(dt1);
                        notf = notf + "Need pay IP:"+r0+" price "+r1+" Low "+ QString::number(days1) +" days \n ";
                    }
                } else {
                    // 14 days
                    dt3 = dt2.addDays(15);
                    if (dt1 < dt3) {
                        if (black_theme == 0) model->setData(index2, QBrush("#FFFFD0"), Qt::BackgroundRole);
                        if (black_theme == 1) model->setData(index2, QBrush("#606040"), Qt::BackgroundRole);

                        if (time_h == "15:00") {
                            index2 = model->index(x,1);
                            r0 = model->data(index2).toString();
                            index2 = model->index(x,4);
                            r1 = model->data(index2).toString();
                            days1 = dt2.daysTo(dt1);
                            notf = notf + "Need pay IP:"+r0+" price "+r1+" Low "+ QString::number(days1) +" days \n ";
                        }
                    }
                }
            }
        }
    }

    if (show_msg == 0) {
        if (notf.length() > 0) {
            trayIcon->showMessage("Notification", notf,QSystemTrayIcon::Information,10000);
            show_msg = 10;

            if (isVisible()) {
            } else {
               showNormal();
               // setWindowState(Qt::WindowState::WindowActive);   // вывести основное окно поверх всех окон... есть минус - сшибает оповещение в трее. И немного неудобно. Если кому это нужно - раскомментировать
            }
            QSound::play(":/snd/snd1.wav");
        }
    }
}

void MainWindow::Start_m1() {
    // Первый старт приложения, и проверим доступность базы данных
    QString cfg = "";
    QFile file("memz.cfg");
    // Если файл memz.cfg существует, и там не пустая строка 1, то со строки 1 будет считано имя базы данных.
    if ((file.exists())&&(file.open(QIODevice::ReadOnly))) {
        cfg = file.readLine();
        file.close();
    }
    if (cfg != "") {
        file_db = cfg.trimmed();
    } else {
        // В файл memz.cfg будет записано значение имени файла базы данных, если в нём еще ничего нет.
        if (file.open(QIODevice::WriteOnly)) {
            file.write(file_db.toUtf8().data());
            //file.write("\n");
            //file.write("-");
            file.close();
        }
    }

    // Если что-то не так с базой данных (нет файла), то откроем форму с созданием базы.
    if (!QFile::exists(file_db)) {
        errbase win2;
        win2.setModal(true);
        win2.exec();
        if (is_base_ok == 3) {
            db1.open();
            is_base_ok = 1;
            QTimer::singleShot(500, this, SLOT(on_pushButton_clicked()));
        }
        if (is_base_ok == 99) {
            edited = 777;
            close();
        }

    } else {
        // Будем считать, что база существует и всё ок
        is_base_ok = 1;
        QTimer::singleShot(200, this, SLOT(on_pushButton_clicked()));
    }
}


void MainWindow::on_pushButton_clicked() {
    if (is_base_ok != 1) {
        QMessageBox::warning(this, "Warning","Database not active!");
        return;
    }
    if (! db1.isValid()) {
        db1 = QSqlDatabase::addDatabase("QSQLITE");
    }

    is_base_ok = 2;

    MainWindow::on_read_db_clicked();
}

void MainWindow::on_read_db_clicked(){
    ui->lineEdit->setText("");

    if (is_base_ok != 2) {
        QMessageBox::warning(this, "Warning","Database not active!");
        return;
    }

    SimpleCrypt crypto;

nopass:

    if (key_x == 0) {
        MainWindow::on_pushButton_3_clicked();
        if (is_base_ok == 99) {
            close();
            return;
        }
        if (is_base_ok == 3) {
            if (db1.isOpen()) db1.close();
            errbase win2;
            win2.setModal(true);
            win2.exec();
            if (is_base_ok == 3) {
                db1.open();
                is_base_ok = 1;
                QTimer::singleShot(500, this, SLOT(on_pushButton_clicked()));
            }
            if (is_base_ok == 99) {
                edited = 777;
                close();
            }
            return;
        }
    }

    if (db1.isOpen()) db1.close();

    db1.setDatabaseName(file_db);
    db1.open();
    is_base_ok = 2;

    crypto.setKey(key_x);

    query = QSqlQuery(db1);
    QString tst = "";
    QString n2 = "";
    int recs_check = 0;
    query.exec("SELECT `id`, `ip` FROM DATA;");
    while (query.next()){
        n2 = query.value(1).toString();
        if (n2.length()>0) recs_check++;
        tst = tst + crypto.decryptToString(n2);
    }

    if (recs_check > 0) {
        if (tst.length() < 2) {
            key_x = 0;
            QMessageBox::warning(this, "Warning","Wrong password (1)");
            goto nopass;
        }
    }

    // сколько записей в хостерах
    query.clear();
    int rec_host = 0;
    query.exec("SELECT `id` FROM HOST;");
    while (query.next()){
        ++rec_host;
    }
    query.clear();

    // считаем всех хостеров
    QString names_url[2][rec_host] = {};
    // Load hosters
    int n0 = 0;
    query.exec("SELECT * FROM HOST;");
    while (query.next()){
        names_url[0][n0] = query.value(0).toString();
        names_url[1][n0] = crypto.decryptToString(query.value(1).toString());
        n0++;
    }

    // сколько записей в курсах валют
    int rec_cur = 0;
    query.clear();
    query.exec("SELECT `id` FROM CUR;");
    while (query.next()){
        ++rec_cur;
    }
    query.clear();
    //


    QString info1[rec_cur];
    int info2[rec_cur];
    int info3[rec_cur];

    int r1 = 0;
    query.exec("SELECT * FROM CUR;");
    while (query.next()){
        info1[r1] = crypto.decryptToString(query.value(1).toString());
        info2[r1] = 0;
        info3[r1] = 0;
        ++r1;
    }
    query.clear();

    int recs = 0;    
    query.exec("SELECT `id` FROM DATA;");
    while (query.next()){
        ++recs;
    }
    query.clear();

    model = new QStandardItemModel(0, cols1, this);
    ui->tableView->setModel(model);
    QModelIndex index;

    //model->clear();
    while (model->rowCount() > 0) model->removeRow(0);

    int r5=0;
    for (r5=0; r5<cols1; r5++) {
        model->setHeaderData(r5, Qt::Horizontal, namescol[r5]);
    }

    for (r5=0; r5<cols1; r5++) {
        ui->tableView->setColumnWidth(r5,width_col[r5]);
    }

    count_rows = 0;
    if (recs < 1) {
        hide_after_start = 0;
        return;
    }

    QString info[cols1][recs];
    QString valuts_in_row[recs];
    QString price_in_row[recs];
    QDate dt1;
    QDate dt3;
    int pf = 0;
    int all_srv = 0;

    r1 = 0;
    QString bb1 = "";
    query.exec("SELECT `id`, `ip`, `country`, `date2`, `price`, `url1`, `info`, `cur` FROM DATA;");
    while (query.next()){

        info[0][r1] = query.value(0).toString();
        info[1][r1] = crypto.decryptToString(query.value(1).toString());  // IP address
        info[2][r1] = crypto.decryptToString(query.value(2).toString());  // Country

        dt1 = query.value(3).toDate();
        info[3][r1] = dt1.toString("dd-MM-yyyy");

        valuts_in_row[r1] = crypto.decryptToString(query.value(7).toString());
        price_in_row[r1] = crypto.decryptToString(query.value(4).toString());
        info[4][r1] = price_in_row[r1] + " " + valuts_in_row[r1];

        n0 = query.value(5).toInt();
        bb1 = "-";
        for (int n1=0;n1<rec_host;n1++){
            if (names_url[0][n1].toInt() == n0) {
                bb1 = names_url[1][n1];
            }
        }

        info[5][r1] = bb1;
        info[6][r1] = crypto.decryptToString(query.value(6).toString());  // Information
        ++r1;
        ++all_srv;
    }

    model->setRowCount(r1);
    count_rows = r1;

    QDate dt2 = QDate::currentDate();

    for (int y=0; y<r1;++y) {
        for (int x=0;x<cols1;++x){
            index = model->index(y,x);
            model->setData(index, info[x][y]);

            if (ax_n[x] == 1) model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
            if (ax_n[x] == 2) model->setData(index, Qt::AlignRight, Qt::TextAlignmentRole);
            if (ax_n[x] == 3) {
                model->setData(index, Qt::AlignCenter, Qt::TextAlignmentRole);
                model->setData(index, "*****");
            }
        }

        //dt1 = QDate::fromString(info[3][y], "dd-MM-yyyy");
        //if (dt1 >= dt2) {
        //    for (int v=0;v<rec_cur;++v){
        //        info2[v] = "";
                //if (info[8][y] == info1[v]) {
                //    info2[v] = info2[v] + info[7][y].toInt();
               // }
        //    }
       // }

        dt1 = QDate::fromString(info[3][y], "dd-MM-yyyy");
        pf = dt2.daysTo(dt1);
        if (dt1 >= dt2) {
            for (int v=0;v<rec_cur;++v){
                if (valuts_in_row[y] == info1[v]) {
                   info2[v] = info2[v] + price_in_row[y].toInt();
                   }
            }
            if (pf < 15) {
                //qDebug() << query.lastError().text();
                //qDebug() << pf;
                for (int v=0;v<rec_cur;++v){
                    if (valuts_in_row[y] == info1[v]) {
                       info3[v] = info3[v] + price_in_row[y].toInt();
                       }
                }
            }
            //

        }
        //count_rows

    }
    show_msg = 0;

    QString aaa = "Total servers: "+QString::number(all_srv)+" | Total per month: ";
    QString aab = "| Total expiring: ";
    for (int v=0;v<rec_cur;++v){
        if (info2[v] > 0) {
            aaa = aaa + QString::number(info2[v]) +" "+info1[v]+" ";
        }
        if (info3[v] > 0) {
            aab = aab + QString::number(info3[v]) +" "+info1[v]+" ";
        }
    }
    if (aab.length() < 20) aab = "";

    ui->info1->setText(aaa+" "+aab);

    if (hide_after_start == 1) {
        hide_after_start = 0;

        if (isVisible()) {
            hide();
        }
    }

}

void MainWindow::on_pushButton_2_clicked() {
    if (is_base_ok != 2) {
        QMessageBox::warning(this, "Warning","Database not active!");
        return;
    }

    //Test password
    if (is_blocked_pass > 0) {
        QString test1 = QInputDialog::getText(this, tr("Need unblock"), tr("Enter password"), QLineEdit::Normal, "");
        if (test1 == "") return;
        if ((test1+key2) != passw_x) {
            QMessageBox::warning(this, "Warning","Wrong password");
            return;
        }
        is_blocked_pass = 0;
        setWindowTitle("Memorize");
    }

    res_edit = 0;
    EditRec win3;
    win3.setModal(true);
    win3.exec();

    if (res_edit == 5) {
        MainWindow::on_read_db_clicked();
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index) {

    Qt::KeyboardModifiers  kbrdMod = qApp->keyboardModifiers();
    bool isSh = (kbrdMod & Qt::ShiftModifier) ? true : false;
    if (isSh) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(model->data(index).toString());
        return;
    }

    //Test password
    if (is_blocked_pass > 0) {
        QString test1 = QInputDialog::getText(this, tr("Need unblock"), tr("Enter password"), QLineEdit::Normal, "");
        if (test1 == "") return;
        if ((test1+key2) != passw_x) {
            QMessageBox::warning(this, "Warning","Wrong password");
            return;
        }
        is_blocked_pass = 0;
        setWindowTitle("Memorize");
    }

    int x = index.row();
    QModelIndex index2 = model->index(x,0);
    res_edit = model->data(index2).toInt();
    EditRec win3;
    win3.setModal(true);
    win3.exec();

    if (res_edit == 5) {
        MainWindow::on_read_db_clicked();
    }
}

void MainWindow::on_pushButton_3_clicked() {
    Setup win5;
    win5.setModal(true);
    win5.exec();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index){
    Qt::KeyboardModifiers  kbrdMod = qApp->keyboardModifiers();
    bool isSh = (kbrdMod & Qt::ShiftModifier) ? true : false;
    if (isSh) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(model->data(index).toString());
        return;
    }
}

void MainWindow::on_pushButton_4_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Close", "Do you want to close Memorize?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        edited = 777;
        close();
    }
}

void MainWindow::copycell() {
    QClipboard *clipboard = QApplication::clipboard();
    QModelIndex index;
    index = model->index(sel_cell_y,sel_cell_x);
    clipboard->setText(model->data(index).toString());
}

void MainWindow::copyrow() {

    //Test password
    if (is_blocked_pass > 0) {
        QString test1 = QInputDialog::getText(this, tr("Need unblock"), tr("Enter password"), QLineEdit::Normal, "");
        if (test1 == "") return;
        if ((test1+key2) != passw_x) {
            QMessageBox::warning(this, "Warning","Wrong password");
            return;
        }
        is_blocked_pass = 0;
        setWindowTitle("Memorize");
    }

    QClipboard *clipboard = QApplication::clipboard();
    QString res = "";
    QModelIndex index;
    index = model->index(sel_cell_y,0);
    int load_it = model->data(index).toInt();
    index = model->index(sel_cell_y,5);
    QString hoster_name = model->data(index).toString();

    SimpleCrypt crypto;
    crypto.setKey(key_x);

    query = QSqlQuery(db1);
    query.clear();

    query.exec("SELECT * FROM DATA WHERE `id` = "+QString::number(load_it)+" ;");
    if (query.next()){
        res = res + "IP address: "+crypto.decryptToString(query.value(1).toString())+"\r\n";
        res = res + "Login: "+crypto.decryptToString(query.value(3).toString())+"\r\n";
        res = res + "Password: "+crypto.decryptToString(query.value(4).toString())+"\r\n";
        res = res + "Price: "+crypto.decryptToString(query.value(7).toString())+" "+crypto.decryptToString(query.value(8).toString())+"\r\n";
        res = res + "------------------\r\n";
        res = res + "First pay: "+query.value(5).toString()+"\r\n";
        res = res + "Next pay: "+query.value(6).toString()+"\r\n";
        res = res + "------------------\r\n";
        res = res + "Provider: "+hoster_name+"\r\n";
        res = res + "login: "+crypto.decryptToString(query.value(10).toString())+"\r\n";
        res = res + "Password: "+crypto.decryptToString(query.value(11).toString())+"\r\n";
        res = res + "------------------\r\n";
        res = res + "Email: "+crypto.decryptToString(query.value(12).toString())+"\r\n";
        res = res + "Passw_email: "+crypto.decryptToString(query.value(13).toString())+"\r\n";
        res = res + "------------------\r\n";
        res = res + "Information: \r\n"+crypto.decryptToString(query.value(14).toString())+"\r\n";
        res = res + "===";

        clipboard->setText(res);
    } else {
        QMessageBox::information(this, "Warning","Something wrong. Please try later.");
    }
    query.clear();

}

void MainWindow::deleterow() {
    QModelIndex index;
    index = model->index(sel_cell_y,0);
    QString id_rec = model->data(index).toString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm delete", "Do you want to delete this entry (ID:"+id_rec+") from the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        query = QSqlQuery(db1);
        query.prepare("DELETE FROM DATA WHERE `id` = :id;");
        query.bindValue(":id", id_rec);
        if (query.exec()) {
            QMessageBox::information(this, "Complete","Record has been deleted.");
            QTimer::singleShot(300, this, SLOT(on_read_db_clicked()));
        } else {
            QMessageBox::information(this, "Warning","An error occurred while deleting the entry.");
        }
    }
}


void MainWindow::on_lineEdit_textEdited(const QString &arg1){
    QString str_f = ui->lineEdit->text();
    QString cell_txt = "";
    int rows = ui->tableView->model()->rowCount();
    QModelIndex index;
    int vis1 = 0;

    for (int a=0; a<rows; a++) {
        vis1 = 0;

        index = model->index(a,1);
        cell_txt = model->data(index).toString();
        if (cell_txt.indexOf(str_f) > -1) vis1 = 1;

        index = model->index(a,6);
        cell_txt = model->data(index).toString();
        if (cell_txt.indexOf(str_f) > -1) vis1 = 1;

        if (vis1 > 0) {
            ui->tableView->showRow(a);
        } else {
            ui->tableView->hideRow(a);
        }
    }
}
