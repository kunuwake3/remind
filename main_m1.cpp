// main_m1_secure.cpp — версия с секретом из GitHub Actions без «магических» литералов

#include "main_m1.h"          // ваш главный заголовок
#include <QApplication>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QtSql>
#include <QCryptographicHash>
#include <QtEndian>
#include <cstdlib>              // qgetenv
#include <stdexcept>
#include <cstring>              // explicit_bzero / memset
#include <QMessageBox>
#include <QSqlError>

//--------------------------------
// Глобальные переменные и настройки
//--------------------------------

QString file_db = "f3_base.db";
int is_base_ok   = 0;
int res_edit     = 0;
int res_edit_c1  = 0;
int show_msg     = 0;
int count_rows   = 0;
int black_theme  = 1;
int edited       = 0;
int sound_type   = 0;
int sel_cell_x   = 0;
int sel_cell_y   = 0;
int nowedithst   = 0;
int timer_hidez  = 0;
int is_blocked_pass = 0;
int cols1        = 7;
int width_col[7] = {38,140,90,110,90,185,320};
QString namescol[7] = {"ID","IP","Country","Next pay","Price","Provider","Information"};
int width_2col[4] = {35,170,225,435};
QString names2col[4] = {"ID","Name","Link","Information"};
int ax_n [7] = {1,0,1,1,0,0,0};

// ----- Убрали фиксированный секрет -----
// QString key2 = "j843gd803d";   // БОЛЬШЕ НЕ НУЖЕН

QString sets3 = "";

/*  odd_ip_input — режимы работы валидатора IP-поля
    0 = разрешён любой ввод, включая текст
    1 = требуется явное нажатие на точку
    2 = ',' автоматически заменяется на '.'
    3 = точки вставляются автоматически
*/
int odd_ip_input = 2;

QString passw_x = "";
static quint64 key_x = 0;     // Заполняется при старте через APP_SECRET_KEY
QString sound_m = "";
QSqlDatabase db1;
QSqlQuery query;

//----------------------------------------------------------
// ИНИЦИАЛИЗАЦИЯ КЛЮЧА ИЗ GITHUB SECRET (APP_SECRET_KEY)
//----------------------------------------------------------

static void initCryptoKey()
{
    QByteArray envVal = qgetenv("APP_SECRET_KEY");
    if (envVal.isEmpty()) {
        throw std::runtime_error("Переменная окружения APP_SECRET_KEY не задана");
    }

    QByteArray hash = QCryptographicHash::hash(envVal, QCryptographicHash::Sha256);
    key_x = qFromBigEndian<quint64>(reinterpret_cast<const unsigned char *>(hash.constData()));

    // Стираем секрет из памяти, чтобы он не остался в дампе
#if defined(__GLIBC__)
    explicit_bzero(envVal.data(), envVal.size());
#else
    volatile char *p = envVal.data();
    std::memset(const_cast<char *>(p), 0, envVal.size());
#endif
}

//----------------------------------------------------------
// СТАРЫЙ МЕТОД: derivation из пользовательского пароля (если нужен)
//----------------------------------------------------------

void getkeypass(const QString &key1)
{
    QByteArray rawHash = QCryptographicHash::hash(key1.toUtf8(), QCryptographicHash::Sha256);
    key_x = qFromBigEndian<quint64>(reinterpret_cast<const unsigned char *>(rawHash.constData()));
}

//----------------------------------------------------------
// ИНИЦИАЛИЗАЦИЯ SQLite БД
//----------------------------------------------------------

static bool openDatabase()
{
    db1 = QSqlDatabase::addDatabase("QSQLITE");
    db1.setDatabaseName(file_db);

    if (!db1.open()) {
        QMessageBox::critical(nullptr,
                              QObject::tr("Ошибка открытия БД"),
                              db1.lastError().text());
        return false;
    }
    is_base_ok = 1;
    return true;
}

//----------------------------------------------------------
// MAIN
//----------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 1. Ключ шифрования из GitHub Secrets
    try {
        initCryptoKey();
    } catch (const std::exception &ex) {
        QMessageBox::critical(nullptr,
                              QObject::tr("Ошибка инициализации ключа"),
                              ex.what());
        return EXIT_FAILURE;
    }

    // 2. Подключаемся к базе данных
    if (!openDatabase()) {
        return EXIT_FAILURE;          // сообщение уже показано в openDatabase()
    }

    // 3. Создаём и отображаем главное окно (класс в main_m1.h)
    Main_M1 mainWindow;               // предполагается, что класс называется Main_M1
    mainWindow.show();

    // 4. Входим в главный цикл
    int ret = app.exec();

    // 5. Корректно закрываем соединение с БД (на случай, если это важно)
    if (db1.isOpen()) {
        db1.close();
    }

    return ret;
}
