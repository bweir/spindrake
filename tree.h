#pragma once

#include <QString>
#include <QList>
#include <QVariant>

#include <stdio.h>

#include "types.h"

class Expr
{
public:
    virtual ~Expr() {}
    virtual void print() = 0;
};


class NumberExpr : public Expr
{
protected:
    quint32 _value;
    int _base;

public:
    virtual ~NumberExpr() {}
    NumberExpr(int base, quint32 value)
    {
        _value = value;
        _base = base;
    }

    void print()
    {
        if (_base == 2) printf("%%");
        else if (_base == 4) printf("%%%%");
        else if (_base == 16) printf("$");
        printf("%s", qPrintable(QString::number(_value, _base)));
    }

    quint32 value()
    {
        return _value;
    }
};

class IdentExpr : public Expr
{
protected:
    QString _value;

public:
    virtual ~IdentExpr() {}
    IdentExpr(QString value)
    {
        _value = value;
    }

    void print()
    {
        printf("%s", qPrintable(_value));
    }

    QString value()
    {
        return _value;
    }
};

class AddressExpr : public Expr
{
protected:
    Expr * _ident;
    Expr * _offset;

public:
    virtual ~AddressExpr() {}
    AddressExpr(Expr * ident, Expr * offset = 0)
    {
        _ident = ident;
        _offset = offset;
    }

    void print()
    {
        if (_offset != 0)
        {
            _ident->print();
            printf("[");
            _offset->print();
            printf("]");
        }
        else
        {
            printf("@");
            _ident->print();
        }
    }
};


class LiteralExpr : public Expr
{
protected:
    Expr * _val;

public:
    virtual ~LiteralExpr() {}
    LiteralExpr(Expr * val)
    {
        _val = val;
    }

    void print()
    {
        printf("#");
        _val->print();
    }
};


class DataTypeExpr : public Expr
{
protected:
    DataType _val;

public:
    virtual ~DataTypeExpr() {}
    DataTypeExpr(DataType val)
    {
        _val = val;
    }

    void print()
    {
        printf("%s ",qPrintable(value()));
    }

    QString value()
    {
        switch (_val)
        {
            case DataByte:  return "byte"; ;;
            case DataWord:  return "word"; ;;
            case DataLong:  return "long"; ;;
            default:        return ""; ;;
        }
    }
};


class BlockExpr : public Expr
{
protected:
    Block _block;
    QList<Expr *> * _lines;

public:
    virtual ~BlockExpr() {}
    BlockExpr(Block block, QList<Expr *> * lines)
    {
        _block = block;
        _lines = lines;
    }

    void print()
    {
        QString s;
        switch (_block)
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

        foreach(Expr * l, *_lines)
        {
            l->print();
        }

    }
};


class DatLineExpr : public Expr
{
protected:
    IdentExpr * _symbol;
    DataTypeExpr * _align;

    QList<Expr *> * _items;

public:
    virtual ~DatLineExpr() {}

    DatLineExpr(Expr * symbol, 
                Expr * align,
                QList<Expr *> * items)
    {
        _symbol = (IdentExpr *) symbol;
        _align = (DataTypeExpr *) align;
        _items = items;
    }

    void print()
    {
        if (!(_symbol->value().isEmpty()))
        {
            printf("\n%s\n", qPrintable(_symbol->value()));
        }
        printf("%-8s", qPrintable(_align->value()));

        for (int i = 0; i < _items->size(); i++)
        {
            (*_items)[i]->print();
            if (i < _items->size()-1)
                printf(", ");
        }
        printf("\n");
    }
};


class UnaryExpr : public Expr
{
protected:
    Expr * _val;
    QString _op;
    bool _post;

public:
    virtual ~UnaryExpr() {}
    UnaryExpr(Expr * val, QString op)
    {
        _val = val;
        _op = op;
        _post = true;
    }

    UnaryExpr(QString op, Expr * val)
    {
        _val = val;
        _op = op;
        _post = false;
    }

    void print()
    {
        if (_post)
        {
            _val->print();
            printf("%s", qPrintable(_op));
        }
        else
        {
            printf("%s", qPrintable(_op));
            _val->print();
        }
    }
};




class BinaryExpr : public Expr
{
protected:
    Expr * _left;
    QString _op;
    Expr * _right;

public:
    virtual ~BinaryExpr() {}
    BinaryExpr(Expr * left, QString op, Expr * right)
    {
        _left = left;
        _op = op;
        _right = right;
    }

    void print()
    {
        _left->print();
        printf(" %s ", qPrintable(_op));
        _right->print();
    }
};


class WrapExpr : public Expr
{
protected:
    QString _left;
    Expr * _val;
    QString _right;

public:
    virtual ~WrapExpr() {}
    WrapExpr(QString left, Expr * val, QString right)
    {
        _left = left;
        _val = val;
        _right = right;
    }

    void print()
    {
        printf("%s", qPrintable(_left));
        _val->print();
        printf("%s", qPrintable(_right));
    }
};

