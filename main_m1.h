#ifndef MAIN_M1_H
#define MAIN_M1_H
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QtSql>
#include <QCryptographicHash>
#include <QProcess>

extern QString file_db;
extern int is_base_ok;
extern int res_edit;
extern int res_edit_c1;
extern int show_msg;
extern int count_rows;
extern QString sound_m;
extern int black_theme;
extern int edited;
extern int sound_type;
extern int odd_ip_input;
extern int nowedithst;
extern int timer_hidez;
extern int is_blocked_pass;

extern int width_col[7];
extern QString namescol[7];
extern int ax_n[7];

extern int width_2col[4];
extern QString names2col[4];
extern int cols1;

extern int sel_cell_x;
extern int sel_cell_y;

extern QSqlDatabase db1;
extern QSqlQuery query;

extern QString key2;
extern QString passw_x;
extern qint64 key_x;

extern QString sets3;

extern void getkeypass(QString key1);

#endif // MAIN_M1_H
