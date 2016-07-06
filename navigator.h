#pragma once

#include "tree.h"

class Navigator : public AbstractVisitor
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
        expr._ident->accept(*this);
        expr._offset->accept(*this);
    }

    void visit(LiteralExpr & expr)
    {
        expr.accept(*_visitor);
        expr._val->accept(*this);
    }

    void visit(DataTypeExpr & expr)
    {
        expr.accept(*_visitor);
    }

    void visit(BlockExpr & expr)
    {
        expr.accept(*_visitor);

        foreach(Expr * l, *expr._lines)
            l->accept(*this);
    }

    void visit(DatLineExpr & expr)
    {
        expr.accept(*_visitor);

        expr._symbol->accept(*this);
        expr._align->accept(*this);
        foreach(Expr * i, *expr._items)
            i->accept(*this);
    }

    void visit(DatItemExpr & expr)
    {
        expr.accept(*_visitor);

        expr._size->accept(*this);
        expr._data->accept(*this);
        expr._count->accept(*this);
    }

    void visit(UnaryExpr & expr)
    {
        expr.accept(*_visitor);
        expr._val->accept(*this);
    }

    void visit(BinaryExpr & expr)
    {
        expr.accept(*_visitor);
        expr._left->accept(*this);
        expr._right->accept(*this);
    }

    void visit(WrapExpr & expr)
    {
        expr.accept(*_visitor);
        expr._val->accept(*this);
    }

    void visit(ObjectExpr & expr)
    {
        expr.accept(*_visitor);
        foreach(Expr * b, *expr._blocks)
            b->accept(*this);
    }

    void visit(ConAssignExpr & expr)
    {
        expr.accept(*_visitor);
        expr._ident->accept(*this);
        expr.expr->accept(*this);
    }

public:
    void walk(Expr * root, AbstractVisitor & visitor)
    {
        _visitor = &visitor;

        root->accept(*this);
    }
};



