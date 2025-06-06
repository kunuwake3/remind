#include "main_m1.h"
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QtSql>
#include <QCryptographicHash>

QString file_db = "f3_base.db";
int is_base_ok = 0;
int res_edit = 0;
int res_edit_c1 = 0;
int show_msg = 0;
int count_rows = 0;
int black_theme = 1;
int edited = 0;
int sound_type = 0;
int sel_cell_x = 0;
int sel_cell_y = 0;
int nowedithst = 0;
int timer_hidez = 0;
int is_blocked_pass = 0;
int cols1 = 7;
int width_col[7] = {38,140,90,110,90,185,320};
QString namescol[7] = {"ID","IP","Country","Next pay","Price","Provider","Information"};
int width_2col[4] = {35,170,225,435};
QString names2col[4] = {"ID","Name","Link","Information"};
int ax_n [7] = {1,0,1,1,0,0,0};

QString key2 = "j843gd803d";
QString sets3 = "";


// ------------------------------------------------------------------------------------------
/*  костыльное расширение, если лень нажимать точку в поле ввода IP адреса.
    0 = IP адрес любой может быть вписан, в том числе и текст
    1 = работает только правило ввода, точку нажимать требуется
    2 = то же самое, что и 1, только при вводе запятой автоматом символ меняет на точку
    3 = точки подставляются автоматом
*/
int odd_ip_input = 2;
// ------------------------------------------------------------------------------------------

QString passw_x = "";
qint64 key_x = 0;
QString sound_m = "";
QSqlDatabase db1;
QSqlQuery query;

// ------------------------------------------------------------------------------------------
void getkeypass(QString key1) {
    QCryptographicHash hash1(QCryptographicHash::Sha256);
    QByteArray key2 = key1.toLocal8Bit();
    const char *keys = key2.data();
    hash1.addData(keys);
    key_x = qFromBigEndian<quint64>((const uchar*)hash1.result().toBase64().constData());
}
