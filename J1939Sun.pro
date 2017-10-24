#-------------------------------------------------
#
# Project created by QtCreator 2017-10-19T17:58:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = J1939Sun
TEMPLATE = app

##说明：根据不同的can盒加载相应的驱动
win32:LIBS += -L$$PWD/lib/controlcan -lControlCAN
DEFINES += USE_CONTROLCAN

#win32:LIBS += -L$$PWD/lib/ECAN -lECanVci
#DEFINES += USE_ECAN

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += lib \
                j1939

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    j1939/J1939.cpp



HEADERS  += src/mainwindow.h \
            lib/can_config.h \
    j1939/J1939.H \
    j1939/J1939_Config.H \
    lib/ECAN/ECanVci.h



##说明：根据不同的can盒加载相应的头文件
HEADERS += lib/controlcan/ControlCAN.h

FORMS    += src/mainwindow.ui


# Objects Pathes
DESTDIR = ./bin
UI_DIR  = ./build
MOC_DIR = ./build
RCC_DIR = ./build
OBJECTS_DIR = ./build
