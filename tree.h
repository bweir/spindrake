#pragma once

#include "types.h"
#include "func.h"


class AbstractVisitor;

class Expr
{
public:
    quint32 num;
    virtual ~Expr() {}
    virtual bool isConstant() = 0;
    virtual quint32 value() = 0;
    virtual void fold() = 0;

    virtual void accept(AbstractVisitor & visitor) = 0;
};

class AbstractVisitor {
public:
    virtual void visit(NumberExpr & expr) = 0;
    virtual void visit(IdentExpr & expr) = 0;
    virtual void visit(AddressExpr & expr) = 0;
    virtual void visit(LiteralExpr & expr) = 0;
    virtual void visit(DataTypeExpr & expr) = 0;
    virtual void visit(BlockExpr & expr) = 0;
    virtual void visit(DatLineExpr & expr) = 0;
    virtual void visit(DatItemExpr & expr) = 0;
    virtual void visit(UnaryExpr & expr) = 0;
    virtual void visit(BinaryExpr & expr) = 0;
    virtual void visit(WrapExpr & expr) = 0;
    virtual void visit(ObjectExpr & expr) = 0;

    virtual void visit(ConAssignExpr & expr) = 0;
};


class NumberExpr : public Expr
{
public:
    int _base;
    virtual ~NumberExpr() {}
    NumberExpr(int base, quint32 value)
    {
        num = value;
        _base = base;
    }

    bool isConstant()
    {
        return true;
    }
    
    quint32 value()
    {
        return num;
    }

    void fold() {}

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


Expr * foldConstants(Expr * exp);



class IdentExpr : public Expr
{
public:
    QString _ident;
    virtual ~IdentExpr() {}
    IdentExpr(QString ident)
    {
        _ident = ident;
    }

    bool isConstant()
    {
        return false;
    }

    QString ident()
    {
        return _ident.toLower();
    }

    quint32 value()
    {
        return 0;
    }

    void fold() {}

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};

class AddressExpr : public Expr
{
public:
    IdentExpr * _ident;
    Expr * _offset;

    virtual ~AddressExpr()
    {
        delete _ident;
        delete _offset;
    }

    AddressExpr(Expr * ident, Expr * offset)
    {
        _ident = (IdentExpr *) ident;
        _offset = offset;
    }

    bool isConstant()
    {
        return false;
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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class LiteralExpr : public Expr
{
public:
    Expr * _val;

    virtual ~LiteralExpr()
    {
        delete _val;
    }

    LiteralExpr(Expr * val)
    {
        _val = val;
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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class DataTypeExpr : public Expr
{
public:
    DataType _val;

    virtual ~DataTypeExpr() {}
    DataTypeExpr(DataType val = NoDataType)
    {
        _val = val;
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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class BlockExpr : public Expr
{
public:
    Block _block;
    QList<Expr *> * _lines;

    virtual ~BlockExpr()
    {
        foreach(Expr * l, *_lines) { delete l; }
        delete _lines;
    }

    BlockExpr(Block block, QList<Expr *> * lines)
    {
        _block = block;
        _lines = lines;
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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class DatLineExpr : public Expr
{
public:
    IdentExpr * _symbol;
    DataTypeExpr * _align;

    QList<Expr *> * _items;

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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class DatItemExpr : public Expr
{
public:
    DataTypeExpr * _size;
    Expr * _data;
    Expr * _count;

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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


class UnaryExpr : public Expr
{
public:
    Expr * _val;
    QString _op;
    bool _post;

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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};




class BinaryExpr : public Expr
{
public:
    Expr * _left;
    QString _op;
    Expr * _right;

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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};



class ConAssignExpr : public Expr
{
public:
    IdentExpr * _ident;
    Expr * expr;

    virtual ~ConAssignExpr()
    {
        delete expr;
    }

    ConAssignExpr(Expr * ident, Expr * expr)
    {
        _ident = (IdentExpr *) ident;
        this->expr = expr;
    }

    quint32 value()
    {
        if (isConstant())
            return expr->value();
        else
            return 0;
    }

    bool isConstant()
    {
        return expr->isConstant();
    }

    void fold()
    {
        expr = foldConstants(expr);
    }

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};



class WrapExpr : public Expr
{
public:
    QString _left;
    Expr * _val;
    QString _right;

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

    bool isConstant()
    {
        return false; //_val->isConstant();
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

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};



class ObjectExpr : public Expr
{
public:
    QString name;
    QList<Expr *> * _blocks;

    virtual ~ObjectExpr()
    {
        foreach(Expr * b, *_blocks) { delete b; }
        delete _blocks;
    }

    ObjectExpr(QString name, QList<Expr *> * blocks)
    {
        this->name = name;
        _blocks = blocks;
    }

    bool isConstant()
    {
        return false;
    }

    quint32 value()
    {
        return 0;
    }

    void fold()
    {
        foreach(Expr * b, *_blocks) { b->fold(); }
    }

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};


/*class ObjectExpr : public Expr
 *
void deleteHash(QHash<QString, Expr *> & hash);
{
public:
    QString name;

    QHash<QString, Expr *> objects;
    QHash<QString, Expr *> constants;
    QHash<QString, Expr *> variables;
    QHash<QString, Expr *> data;

    QList<Expr *> pubfunctions;
    QList<Expr *> prifunctions;

    virtual ~ObjectExpr()
    {
        deleteHash(objects);
        deleteHash(constants);
        deleteHash(variables);
        deleteHash(data);

        foreach(Expr * i, pubfunctions) delete i;
        foreach(Expr * i, prifunctions) delete i;
    }

    ObjectExpr(QString name)
    {
        this->name = name;
    }

    bool isConstant()
    {
        return false;
    }

    quint32 value()
    {
        return 0;
    }


    void fold()
    {
        foreach(Expr * i, constants)    i->fold();
        foreach(Expr * i, data)         i->fold();
        foreach(Expr * i, pubfunctions) i->fold();
        foreach(Expr * i, prifunctions) i->fold();
    }

    void accept(AbstractVisitor & visitor) override { visitor.visit(*this); }
};

*/
