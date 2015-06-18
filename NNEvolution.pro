#-------------------------------------------------
#
# Project created by QtCreator 2015-06-14T19:47:26
#
#-------------------------------------------------

QT       += core gui concurrent widgets

CONFIG += c++11
TARGET = NNEvolution
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    entity.cpp \
    neuralnetwork.cpp \
    map.cpp \
    mapviewwidget.cpp

HEADERS  += mainwindow.h \
    entity.h \
    neuralnetwork.h \
    action.h \
    map.h \
    defs.h \
    mapviewwidget.h

FORMS    += mainwindow.ui
