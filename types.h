#pragma once

#include <QString>
#include <QList>
#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <stdio.h>

class Expr;
class NumberExpr;
class IdentExpr;
class AddressExpr;
class LiteralExpr;
class DataTypeExpr;
class BlockExpr;
class DatLineExpr;
class DatItemExpr;
class UnaryExpr;
class BinaryExpr;
class WrapExpr;
class ObjectExpr;
class ConAssignExpr;

enum DataType {
    NoDataType,
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


typedef struct newLLType
{  
  int first_line;  
  int first_column;  
  int last_line;  
  int last_column;  
  QString line;
} newLLType;

#define YYLTYPE newLLType
#define YYLTYPE_IS_DECLARED 1

