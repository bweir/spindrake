#pragma once

#include "tree.h"

class Printer : public AbstractVisitor
{
    void visit(NumberExpr & expr)
    {
        switch (expr._base)
        {
            case 2: printf("%%");
            case 4: printf("%%%%");
            case 16: printf("$");
        }
        if (expr._base == 10)
            printf("%s", qPrintable(QString::number((qint32) expr.num)));
        else
            printf("%s", qPrintable(QString::number(expr.num, expr._base)));
    }

    void visit(IdentExpr & expr)
    {
        printf("%s", qPrintable(expr._ident));
    }

    void visit(AddressExpr & expr)
    {
        if (expr._offset->value() != 0)
        {
            expr._ident->accept(*this);
            printf("[");
            expr._offset->accept(*this);
            printf("]");
        }
        else
        {
            printf("@");
            expr._ident->accept(*this);
        }
    }

    void visit(LiteralExpr & expr)
    {
        printf("#");
        expr._val->accept(*this);
    }

    void visit(DataTypeExpr & expr)
    {
        printf("%s",qPrintable(expr.ident()));
    }

    void visit(BlockExpr & expr)
    {
        QString s;
        switch (expr._block)
        {
            case NoBlock:  break; ;;
            case ConBlock: s = "CON"; break; ;;
            case VarBlock: s = "VAR"; break; ;;
            case ObjBlock: s = "OBJ"; break; ;;
            case PubBlock: s = "PUB"; break; ;;
            case PriBlock: s = "PRI"; break; ;;
            case DatBlock: s = "DAT"; break; ;;
            case AsmBlock: s = "ASM"; break; ;;
        }
        printf("%s\n",qPrintable(s));

        foreach(Expr * l, *expr._lines)
        {
            printf("    ");
            l->accept(*this);
            printf("\n");
        }
        printf("\n");
    }

    void visit(DatLineExpr & expr)
    {
        if (!(expr._symbol->ident().isEmpty()))
            printf("%s\n    ", qPrintable(expr._symbol->ident()));

        printf("%-8s", qPrintable(expr._align->ident()));

        for (int i = 0; i < expr._items->size(); i++)
        {
            (*expr._items)[i]->accept(*this);
            if (i < expr._items->size()-1)
                printf(", ");
        }
    }

    void visit(DatItemExpr & expr)
    {
        if (!(expr._size->ident().isEmpty()))
        {
            expr._size->accept(*this);
            printf(" ");
        }

        expr._data->accept(*this);

        if (expr._count->value())
        {
            printf(" ");
            expr._count->accept(*this);
        }
    }

    void visit(UnaryExpr & expr)
    {
        if (expr._post)
        {
            expr._val->accept(*this);
            printf("%s", qPrintable(expr._op));
        }
        else
        {
            printf("%s", qPrintable(expr._op));
            expr._val->accept(*this);
        }
    }

    void visit(BinaryExpr & expr)
    {
        expr._left->accept(*this);
        printf(" %s ", qPrintable(expr._op));
        expr._right->accept(*this);
    }

    void visit(WrapExpr & expr)
    {
        printf("%s", qPrintable(expr._left));
        expr._val->accept(*this);
        printf("%s", qPrintable(expr._right));
    }

    void visit(ObjectExpr & expr)
    {
        foreach(Expr * b, *expr._blocks)
        {
            b->accept(*this);
        }
    }

    void visit(ConAssignExpr & expr)
    {
        expr._ident->accept(*this);
        printf(" = ");
        expr.expr->accept(*this);
    }


public:
    void print(Expr * root)
    {
        root->accept(*this);
    }
};


