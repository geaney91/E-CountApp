#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T14:18:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = E-CountApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    count.cpp \
    validate.cpp \
    filework.cpp \
    candidate.cpp \
    countdialog.cpp \
    vote.cpp

HEADERS  += mainwindow.h \
    count.h \
    validate.h \
    filework.h \
    candidate.h \
    countdialog.h \
    vote.h

FORMS    += mainwindow.ui \
    countdialog.ui
