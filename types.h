#pragma once

#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QVariant>
#include <stdio.h>


enum DataType {
    DataByte,
    DataWord,
    DataLong
};

enum Block {
    NoBlock,
    ConBlock,
    VarBlock,
    ObjBlock,
    PubBlock,
    PriBlock,
    DatBlock,
    AsmBlock
};

