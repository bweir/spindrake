include(bison.pri)
include(flex.pri)

LIBS += -lfl -ly

TEMPLATE = app
TARGET = spindrake
INCLUDEPATH += .
CONFIG += debug

SOURCES += \
    tree.cpp \
    func.cpp \

HEADERS += \
    types.h \
    tree.h \
    func.h \

FLEXSOURCES += lexer.l
BISONSOURCES += parser.y
