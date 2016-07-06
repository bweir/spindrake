include(bison.pri)
include(flex.pri)

LIBS += -lfl -ly

TEMPLATE = app
TARGET = spindrake
INCLUDEPATH += .
CONFIG += debug
CONFIG += c++11

SOURCES += \
    tree.cpp \
    func.cpp \
    main.cpp \

HEADERS += \
    types.h \
    tree.h \
    printer.h \
    func.h \
    navigator.h \

FLEXSOURCES += lexer.l
BISONSOURCES += parser.y
