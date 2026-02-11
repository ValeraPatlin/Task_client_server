QT += core gui
QT += core network

CONFIG += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ../Message_handler/message_handler.cpp \
        main.cpp \
        window.cpp \
    client.cpp \

HEADERS += \
    ../Message_handler/message_handler.h \
        window.h \
    client.h \

FORMS += \
        window.ui
