#-------------------------------------------------
#
# Project created by QtCreator 2013-06-02T15:12:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StudentManager
TEMPLATE = app


SOURCES += main.cpp\
        StudentMain.cpp \
    LoginDialog.cpp \
    Student.cpp \
    StatisticsDialog.cpp \
    Controller.cpp

HEADERS  += StudentMain.h \
    LoginDialog.h \
    Student.h \
    StatisticsDialog.h \
    Controller.h

FORMS    += StudentMain.ui \
    LoginDialog.ui \
    StatisticsDialog.ui

QMAKE_CXXFLAGS += -std=c++11
