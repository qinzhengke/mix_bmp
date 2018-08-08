QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mix_bmp_viewer
TEMPLATE = app


SOURCES += \
    ../src/main.cpp \
    ../src/rawbmpwidget.cpp \
    ../../mix_bmp/mix_bmp.cpp \
    ../src/genmainwindow.cpp \
    ../src/bmp.cpp

HEADERS  += mainwindow.h \
    ../src/rawbmpwidget.h \
    ../../mix_bmp/mix_bmp.h \
    ../src/genmainwindow.h \
    ../src/bmp.h

INCLUDEPATH += \
    ../../mix_bmp
