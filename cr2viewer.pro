QT += core gui widgets

TARGET = cr2viewer
TEMPLATE = app

LIBS += -lraw

CONFIG += c++11

SOURCES += \
        main.cpp \
        widget.cpp

HEADERS += \
        widget.h

FORMS += \
        widget.ui

RESOURCES += \
    icons.qrc

