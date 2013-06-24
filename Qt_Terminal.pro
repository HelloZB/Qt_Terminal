#-------------------------------------------------
#
# Project created by QtCreator 2013-04-30T08:49:29
#
#-------------------------------------------------

CONFIG += serialport #Qt4

QT       += core gui

 greaterThan(QT_MAJOR_VERSION, 4):  QT += widgets # Qt5 serialport

TARGET = Terminalka #Qt_Terminal
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    checkcurrport.cpp

HEADERS  += widget.h \
    checkcurrport.h

FORMS    += widget.ui

RESOURCES += \
    res.qrc


RC_FILE += file.rc
OTHER_FILES += file.rc
