#pragma once

#include "types.h"
#include "func.h"

class Expr
{
protected:
    quint32 _value;

public:
    virtual ~Expr() {}
    virtual void print() = 0;
    virtual bool isConstant() = 0;
    virtual quint32 value() = 0;
    virtual void fold() = 0;
};


class NumberExpr : public Expr
{
protected:
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
        switch (_base)
        {
            case 2: printf("%%");
            case 4: printf("%%%%");
            case 16: printf("$");
        }
        if (_base == 10)
            printf("%s", qPrintable(QString::number((qint32) _value)));
        else
            printf("%s", qPrintable(QString::number(_value, _base)));
    }

    bool isConstant()
    {
        return true;
    }
    
    quint32 value()
    {
        return _value;
    }

    void fold() {}
};


Expr * foldConstants(Expr * exp);



class IdentExpr : public Expr
{
protected:
    QString _ident;

public:
    virtual ~IdentExpr() {}
    IdentExpr(QString ident)
    {
        _ident = ident;
    }

    void print()
    {
        printf("%s", qPrintable(_ident));
    }

    bool isConstant()
    {
        return false;
    }

    QString ident()
    {
        return _ident;
    }

    quint32 value()
    {
        return 0;
    }

    void fold() {}
};

class AddressExpr : public Expr
{
protected:
    Expr * _ident;
    Expr * _offset;

public:
    virtual ~AddressExpr()
    {
        delete _ident;
        delete _offset;
    }

    AddressExpr(Expr * ident, Expr * offset)
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

    bool isConstant()
    {
        return _offset->isConstant();
    }

    quint32 value()
    {
        if (!isConstant()) return false;
        return _offset->value();
    }

    void fold()
    {
        _offset = foldConstants(_offset);
    }
};


class LiteralExpr : public Expr
{
protected:
    Expr * _val;

public:
    virtual ~LiteralExpr()
    {
        delete _val;
    }

    LiteralExpr(Expr * val)
    {
        _val = val;
    }

    void print()
    {
        printf("#");
        _val->print();
    }

    bool isConstant()
    {
        return _val->isConstant();
    }

    quint32 value()
    {
        if (!isConstant()) return 0;
        return _val->value();
    }

    void fold()
    {
        _val = foldConstants(_val);
    }
};


class DataTypeExpr : public Expr
{
protected:
    DataType _val;

public:
    virtual ~DataTypeExpr() {}
    DataTypeExpr(DataType val = NoDataType)
    {
        _val = val;
    }

    void print()
    {
        printf("%s",qPrintable(value()));
    }

    bool isConstant()
    {
        return true;
    }

    QString ident()
    {
        switch (_val)
        {
            case DataByte:      return "byte"; ;;
            case DataWord:      return "word"; ;;
            case DataLong:      return "long"; ;;
            case NoDataType:    return ""; ;;
        }
        return "";
    }

    quint32 value()
    {
        return 0;
    }

    void fold() {}
};


class BlockExpr : public Expr
{
protected:
    Block _block;
    QList<Expr *> * _lines;

public:
    virtual ~BlockExpr()
    {
        foreach(Expr * l, *_lines)
        {
            delete l;
        }
        delete _lines;
    }

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

    bool isConstant()
    {
        foreach(Expr * l, *_lines)
        {
            if (!l->isConstant())
                return false;
        }

        return true;
    }

    quint32 value()
    {
        return 0;
    }

    void fold()
    {
        foreach(Expr * l, *_lines)
        {
            l->fold();
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
    virtual ~DatLineExpr()
    {
        delete _symbol;
        delete _align;

        foreach(Expr * i, *_items)
        {
            delete i;
        }
        delete _items;
    }

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
        if (!(_symbol->ident().isEmpty()))
            printf("\n%s\n", qPrintable(_symbol->ident()));

        printf("%-8s", qPrintable(_align->ident()));

        for (int i = 0; i < _items->size(); i++)
        {
            (*_items)[i]->print();
            if (i < _items->size()-1)
                printf(", ");
        }
        printf("\n");
    }

    bool isConstant()
    {
        foreach(Expr * i, *_items)
        {
            if (!i->isConstant())
                return false;
        }

        return true;
    }

    quint32 value()
    {
        return 0;
    }

    void fold()
    {
        foreach(Expr * l, *_items)
        {
            l->fold();
        }
    }
};


class DatItemExpr : public Expr
{
protected:
    DataTypeExpr * _size;
    Expr * _data;
    Expr * _count;

public:
    virtual ~DatItemExpr()
    {
        delete _size;
        delete _data;
        delete _count;
    }

    DatItemExpr(Expr * size, Expr * data, Expr * count)
    {
        _size = (DataTypeExpr *) size;
        _data = (Expr *) data;
        _count = (Expr *) count;
    }

    void print()
    {
        if (!(_size->ident().isEmpty()))
        {
            _size->print();
            printf(" ");
        }

        _data->print();

        if (_count->value())
        {
            printf(" ");
            _count->print();
        }
    }

    bool isConstant()
    {
        if (!_size->isConstant()) return false;
        if (!_data->isConstant()) return false;
        if (!_count->isConstant()) return false;
        return true;
    }

    quint32 value()
    {
        if (!_data->isConstant()) return 0;
        return _data->value();
    }

    void fold()
    {
        _data = foldConstants(_data);
        _count = foldConstants(_count);
    }
};


class UnaryExpr : public Expr
{
protected:
    Expr * _val;
    QString _op;
    bool _post;

public:
    virtual ~UnaryExpr()
    {
        delete _val;
    }

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

    bool isConstant()
    {
        return _val->isConstant();
    }

    quint32 value()
    {
        if (!isConstant()) return 0;

        quint32 v = _val->value();

        if (_post)
        {
            if (_op == "++")        return v++;
            else if (_op == "--")   return v--;
            else if (_op == "~~")   return v = 0xffffffff;
            else if (_op == "~")    return v = 0x00000000;
            else return 0;
        }
        else
        {
            if (_op == "not" )      return !v;
            else if (_op == "-")    return -v;
            else if (_op == "!")    return ~v;
            else if (_op == "++")   return ++v;
            else if (_op == "--")   return --v;
            else if (_op == "~~")   { if (v & (1 << 15)) v |= 0xffff0000; return v; }
            else if (_op == "~")    { if (v & (1 << 7))  v |= 0xffffff00; return v; }
            else return 0;
        }
    }

    void fold()
    {
        _val = foldConstants(_val);
    }
};




class BinaryExpr : public Expr
{
protected:
    Expr * _left;
    QString _op;
    Expr * _right;

public:
    virtual ~BinaryExpr()
    {
        delete _left;
        delete _right;
    }
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

    bool isConstant()
    {
        if (!_left->isConstant()) return false;
        if (!_right->isConstant()) return false;
        return true;
    }

    quint32 value()
    {
        if (!isConstant()) return 0;

        quint32 l = _left->value();
        quint32 r = _right->value();

        if (_op == "=" )        return l = r;
        else if (_op == "+=")   return l = l + r;
        else if (_op == "-=")   return l = l - r;
        else if (_op == "*=")   return l = l * r;
        else if (_op == "//=")  return l = l % r;
        else if (_op == "/=")   return l = l / r;
        else if (_op == "<<=")  return l = l << r;
        else if (_op == ">>=")  return l = l >> r;
        else if (_op == "~>=")  return l = (quint32) (((qint32) l) >> r);
        else if (_op == "<-=")  return l = rotateLeft(l, r);
        else if (_op == "->=")  return l = rotateLeft(l, r);
        else if (_op == "><=")  return l = reverse(l, r);
        else if (_op == "&=")   return l = l & r;
        else if (_op == "|=")   return l = l | r;
        else if (_op == "^=")   return l = l ^ r;

        else if (_op == "or")   return l || r;
        else if (_op == "and")  return l && r;

        else if (_op == "==")   return (l == r);
        else if (_op == "<>")   return (l != r);
        else if (_op == "<")    return (l < r);
        else if (_op == ">")    return (l > r);
        else if (_op == "<=")   return (l <= r);
        else if (_op == ">=")   return (l >= r);

        else if (_op == "+")    return l + r;
        else if (_op == "-")    return l - r;

        else if (_op == "*")    return l * r;
        else if (_op == "//")   return l % r;
        else if (_op == "/")    return l / r;

        else if (_op == "|")    return l | r;
        else if (_op == "^")    return l ^ r;

        else if (_op == "&")    return l & r;

        else if (_op == "<<")   return l << r;
        else if (_op == ">>")   return l >> r;
        else if (_op == "~>")   return (quint32) (((qint32) l) >> r);
        else if (_op == "<-")   return rotateLeft(l, r);
        else if (_op == "->")   return rotateRight(l, r);
        else if (_op == "><")   return reverse(l, r);

        else return 0;
    }

    void fold()
    {
        _left = foldConstants(_left);
        _right = foldConstants(_right);
    }
};


class WrapExpr : public Expr
{
protected:
    QString _left;
    Expr * _val;
    QString _right;

public:
    virtual ~WrapExpr()
    {
        delete _val;
    }
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

    bool isConstant()
    {
        return _val->isConstant();
    }

    quint32 value()
    {
        if (!_val->isConstant()) return 0;
        return _val->value();
    }

    void fold()
    {
        _val = foldConstants(_val);
    }
};

