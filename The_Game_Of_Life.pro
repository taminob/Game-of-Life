# © Copyright (c) 2017 SqYtCO

QT       += core gui widgets

TARGET = Game_Of_Life
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++14

SOURCES += \
    main.cpp \
    graphicalwindow.cpp \
    graphiccell.cpp \
    mainwindow.cpp \
    settingswindow.cpp \
    label.cpp \
    settings.cpp \
    separatethread.cpp \
    cellsystem.cpp \
    helpwindow.cpp

HEADERS  += \
    cell.h \
    graphicalwindow.h \
    graphiccell.h \
    mainwindow.h \
    settingswindow.h \
    label.h \
    settings.h \
    separatethread.h \
    cellsystem.h \
    helpwindow.h

TRANSLATIONS += gol_de.ts

RESOURCES += resources.qrc

DISTFILES += README.md
