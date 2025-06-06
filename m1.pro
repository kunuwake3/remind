#-------------------------------------------------
#
# Project created by QtCreator 2020-02-09T09:42:25
#
#-------------------------------------------------

QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = m1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main_m1.cpp \
        main.cpp \
        mainwindow.cpp \
        errbase.cpp \
    editrec.cpp \
    editcountry.cpp \
    editcur.cpp \
    messageslist.cpp \
    setup.cpp \
    editprv.cpp \
    simplecrypt.cpp

HEADERS += \
        main_m1.h \
        mainwindow.h \
        errbase.h \
    editrec.h \
    editcountry.h \
    editcur.h \
    messageslist.h \
    setup.h \
    editprv.h \
    simplecrypt.h

FORMS += \
        mainwindow.ui \
        errbase.ui \
    editrec.ui \
    editcountry.ui \
    editcur.ui \
    messageslist.ui \
    setup.ui \
    editprv.ui

RESOURCES += \
    img.qrc
