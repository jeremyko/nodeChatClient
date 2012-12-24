#-------------------------------------------------
#
# Project created by QtCreator 2012-12-12T12:35:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nodeChatClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chatdialog.cpp \
    logindialog.cpp \
    netmanager.cpp \
    registerdialog.cpp \
    addnewfrienddialog.cpp

HEADERS  += mainwindow.h \
    chatdialog.h \
    logindialog.h \
    netmanager.h \
    packets.h \
    registerdialog.h \
    mytreewidget.h \
    addnewfrienddialog.h

FORMS    += mainwindow.ui \
    chatdialog.ui \
    logindialog.ui \
    registerdialog.ui \
    addnewfrienddialog.ui \
    LogInDialog.ui
