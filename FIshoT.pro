#-------------------------------------------------
#
# Project created by QtCreator 2016-08-24T13:57:09
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIshoT
TEMPLATE = app

VERSION = 1.0

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APPLICATION_NAME=\\\"FIshoT\\\"
DEFINES += ORGANIZATION_NAME=\\\"GreenNatureLAB\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    serial.cpp

HEADERS  += mainwindow.h \
    serial.h

FORMS    += mainwindow.ui \
    about.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    Resource/1473258333_gear.ico \
    Resource/1474287311_electronics-06.ico \
    Resource/Ico.ico \
    Resource/wifi-0.ico \
    Resource/wifi-100.ico \

