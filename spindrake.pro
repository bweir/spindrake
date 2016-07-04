include(bison.pri)
include(flex.pri)

LIBS += -lfl -ly

TEMPLATE = app
TARGET = spindrake
INCLUDEPATH += .

HEADERS += \
    types.h \
    tree.h \

FLEXSOURCES += lexer.l
BISONSOURCES += parser.y
