#-------------------------------------------------
#
# Project created by QtCreator 2017-05-01T21:17:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Game_Of_Life
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++14


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    graphicalwindow.cpp \
    graphiccell.cpp \
    main.cpp \
    mainwindow.cpp \
    settingswindow.cpp \
    label.cpp \
    settings.cpp \
    separatethread.cpp \
    cellsystem.cpp

HEADERS  += \
    cell.h \
    graphicalwindow.h \
    graphiccell.h \
    mainwindow.h \
    settingswindow.h \
    label.h \
    settings.h \
    separatethread.h \
    cellsystem.h

TRANSLATIONS += gol_en.ts gol_de.ts

RESOURCES += \
    resources.qrc

DISTFILES += \
    README.md
