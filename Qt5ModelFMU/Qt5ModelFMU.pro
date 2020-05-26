DEFINES += QMODELFMU_LIB

TEMPLATE = lib

QT -= gui

CONFIG += c++14

TARGET = Qt5ModelFMU

DESTDIR = ../../lib

CONFIG(debug, debug|release) {

    TARGET = $$join(TARGET,,,d)

} else {


}

win32 {

    INCLUDEPATH += $$(FMI_LIB_ROOT)/include
    LIBS += -L$$(FMI_LIB_ROOT)/lib -lfmilib_shared
}

unix {

    LIBS += -lfmilib_shared
}

INCLUDEPATH += ./include

HEADERS += $$files(./include/*.h) \
    include/fmi2-variable.h
SOURCES += $$files(./src/*.cpp)
