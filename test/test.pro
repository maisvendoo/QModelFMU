TEMPLATE = app

QT += core
QT -= gui

CONFIG += console
CONFIG += c++14
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR = ../../bin

TARGET = test

CONFIG(debug, debug|release) {

    TARGET = $$join(TARGET,,,_d)

    LIBS += -L../../lib -lQt5ModelFMUd

} else {

    LIBS += -L../../lib -lQt5ModelFMU
}

win32 {

    INCLUDEPATH += $$(FMI_LIB_ROOT)/include
    LIBS += -L$$(FMI_LIB_ROOT)/lib -lfmilib_shared
}

unix {

    LIBS += -lfmilib_shared
}

INCLUDEPATH += ./include
INCLUDEPATH += ../Qt5ModelFMU/include

HEADERS += $$files(./include/*.h)
SOURCES += $$files(./src/*.cpp)
