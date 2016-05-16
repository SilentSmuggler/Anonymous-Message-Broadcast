TEMPLATE = app
TARGET = AnonymousClient
QT += core \
    gui \
    network \
    xml
CONFIG += crypto
HEADERS += anonymousclient.h \
    mainwindow.h \
    debugdialog.h
SOURCES += anonymousclient.cpp \
    main.cpp \
    mainwindow.cpp \
    debugdialog.cpp
FORMS += mainwindow.ui \
    debugdialog.ui
RESOURCES += icons.qrc
