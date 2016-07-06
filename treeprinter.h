#pragma once

#include "tree.h"

class TreePrinter : public AbstractVisitor
{
    void print(QString s, qint32 v)
    {
        printf("(%s: %i)\n", qPrintable(s), v);
    }

public:
    void visit(NumberExpr & expr)
    {
        print("NumberExpr", expr.value());
    }

    void visit(IdentExpr & expr)
    {
        print("IdentExpr", expr.value());
    }

    void visit(AddressExpr & expr)
    {
        print("AddressExpr", expr.value());
    }

    void visit(LiteralExpr & expr)
    {
        print("LiteralExpr", expr.value());
    }

    void visit(DataTypeExpr & expr)
    {
        print("DataTypeExpr", expr.value());
    }

    void visit(BlockExpr & expr)
    {
        print("BlockExpr", expr.value());
    }

    void visit(DatLineExpr & expr)
    {
        print("DatLineExpr", expr.value());
    }

    void visit(DatItemExpr & expr)
    {
        print("DatItemExpr", expr.value());
    }

    void visit(UnaryExpr & expr)
    {
        print("UnaryExpr", expr.value());
    }

    void visit(BinaryExpr & expr)
    {
        print("BinaryExpr", expr.value());
    }

    void visit(WrapExpr & expr)
    {
        print("WrapExpr", expr.value());
    }

    void visit(ObjectExpr & expr)
    {
        print("ObjectExpr", expr.value());
    }

    void visit(ConAssignExpr & expr)
    {
        print("ConAssignExpr", expr.value());
    }
};



