QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mix_bmp_viewer
TEMPLATE = app


SOURCES += \
    ../src/main.cpp \
    ../src/mixbmpwidget.cpp \
    ../src/genmainwindow.cpp \
    ../src/bmp.cpp  \
    ../../mix_bmp/mix_bmp.cpp

HEADERS  += \
    ../src/mainwindow.h \
    ../src/mixbmpwidget.h \
    ../src/genmainwindow.h \
    ../src/bmp.h \
    ../../mix_bmp/mix_bmp.h \

INCLUDEPATH += \
    ../../mix_bmp
