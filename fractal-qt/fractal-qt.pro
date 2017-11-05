QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fractal-qt
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    my_opengl_widget.cpp \
    shader.cpp \
    texture_loader.cpp

HEADERS  += mainwindow.h \
    my_opengl_widget.h \
    texture_loader.h \
    shader.hpp

FORMS    += mainwindow.ui

DISTFILES += \
    texture.fsh \
    transform.vsh

QMAKE_CXXFLAGS += -std=c++11
