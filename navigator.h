#pragma once

#include "tree.h"

class SpinNavigator : public AbstractVisitor
{
    AbstractVisitor * _visitor;

    void visit(NumberExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(IdentExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(AddressExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(LiteralExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(DataTypeExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(BlockExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(DatLineExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(DatItemExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(UnaryExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(BinaryExpr & expr)
    {
        expr._left->accept(*_visitor);
        expr.accept(*_visitor);
        expr._right->accept(*_visitor);
    }
    void visit(WrapExpr & expr)
    {
        expr.accept(*_visitor);
    }
    void visit(ObjectExpr & expr)
    {
        expr.accept(*_visitor);
    }

    void visit(ConAssignExpr & expr)
    {
        expr.accept(*_visitor);
    }

public:
    void walk(Expr * root, AbstractVisitor & visitor)
    {
        _visitor = &visitor;

        root->accept(*_visitor);
    }
};



