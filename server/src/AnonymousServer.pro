TEMPLATE = app
TARGET = AnonymousServer
QT += core \
    gui \
    network \
    xml
CONFIG += crypto
SOURCES += main.cpp \
    mainwindow.cpp \
    anonymousserver.cpp \
    clientsocket.cpp \
    debugdialog.cpp
HEADERS += mainwindow.h \
    anonymousserver.h \
    clientsocket.h \
    debugdialog.h
FORMS += mainwindow.ui \
    debugdialog.ui
RESOURCES += icons.qrc
