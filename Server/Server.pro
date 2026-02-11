QT -= gui
QT += core network

CONFIG += c++17 console
CONFIG -= app_bundle


DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ../Message_handler/message_handler.cpp \
        main.cpp \
    server.cpp \

HEADERS += \
    ../Message_handler/message_handler.h \
    server.h \
