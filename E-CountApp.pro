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
    validate.cpp \
    filework.cpp \
    candidate.cpp \
    countdialog.cpp \
    vote.cpp \
    votelistitem.cpp \
    trackvote.cpp \
    stv.cpp \
    count.cpp \
    writetologfile.cpp \
    resultsform.cpp

HEADERS  += mainwindow.h \
    validate.h \
    filework.h \
    candidate.h \
    countdialog.h \
    vote.h \
    votelistitem.h \
    trackvote.h \
    stv.h \
    count.h \
    writetologfile.h \
    resultsform.h

FORMS    += mainwindow.ui \
    countdialog.ui \
    resultsform.ui
