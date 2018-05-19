QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mix_bmp_viewer
TEMPLATE = app


SOURCES += \
    ../src/main.cpp \
    ../src/rawbmpwidget.cpp \
    ../src/mix_bmp.cpp

HEADERS  += mainwindow.h \
    ../src/rawbmpwidget.h \
    ../src/mix_bmp.h
