QT += core gui widgets

CONFIG += c++17

TARGET = LIA_GUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../LIA/LIA.cpp

HEADERS += \
    mainwindow.h

DEFINES += LIA_GUI_MODE

QMAKE_LFLAGS += -municode