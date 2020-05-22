DEFINES += QMODELFMU_LIB

TEMPLATE = lib

QT -= gui

CONFIG += c++11

TARGET = Qt5ModelFMU

DESTDIR = ../../lib

CONFIG(debug, debug|release) {

    TARGET = $$join(TARGET,,,d)

} else {


}

win32 {

    INCLUDEPATH += $$(FMI_LIB_ROOT)/include
    LIBS += -L$$(FMI_LIB_ROOT)/lib -lfmilib
}

unix {

    LIBS += -lfmilib
}

INCLUDEPATH += ./include

HEADERS += $$files(./include/*.h)
SOURCES += $$files(./src/*.cpp)
