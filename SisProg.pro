TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        vm.cpp \
    assembler.cpp \
    process.cpp \
    processlist.cpp \
    so.cpp

HEADERS += \
        vm.h \
    assembler.h \
    process.h \
    types.h \
    processlist.h \
    so.h
