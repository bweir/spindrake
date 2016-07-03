#pragma once

#include <QString>
#include <QList>
#include <QVariant>

#include <stdio.h>

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




/*
class DecimalExpr : public Expr
{
public:
    DecimalExpr(Match m)
        : Expr(m, "DEC")
    {
        _value = m.text().toInt();
    }

    void print()
    {
        Expr::print(QString::number(_value.toInt()));
    }
};


class FloatExpr : public Expr
{

public:
    FloatExpr(Match m)
        : Expr(m, "FLOAT")
    {
        _value = m.text().toFloat();
    }

    void print()
    {
        Expr::print(QString::number(_value.toFloat()));
    }
};



class HexadecimalExpr : public Expr
{
public:
    HexadecimalExpr(Match m)
        : Expr(m, "HEX")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 8)
            throw Error("Hexadecimal numbers may not exceed 8 characters!");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 16);

        if (!ok)
            throw Error("Failed to read hexadecimal number!");
    }

    void print()
    {
        Expr::print(QString::number(_value.toUInt(), 16));
    }
};


class BinaryExpr : public Expr
{
public:
    BinaryExpr(Match m)
        : Expr(m, "BIN")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 32)
            throw Error("Binary numbers may not exceed 32 characters.");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 2);

        if (!ok)
            throw Error("Failed to read binary number!");
    }

    void print()
    {
        QString s = QString::number(_value.toUInt(), 2);
        printf("%s '", qPrintable(name()));
        for (int i = 0; i < s.size(); i++)
        {
            printf("%s",qPrintable(s[i]));
            if (i < s.size()-1 && i % 8 == 7)
                printf("_");
        }
        printf("'");
        fflush(stdout);
    }
};



class QuaternaryExpr : public Expr
{
public:
    QuaternaryExpr(Match m)
        : Expr(m, "QUAT")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 32)
            throw Error("Quaternary numbers may not exceed 16 characters!");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 4);

        if (!ok)
            throw Error("Failed to read quaternary number!");
    }

    void print()
    {
        Expr::print(QString::number(_value.toUInt(), 4));
    }
};


class IdentifierExpr : public Expr
{
public:
    IdentifierExpr(Match m)
        : Expr(m, "IDENT")
    {
        _value = m.text();
    }

    void print()
    {
        Expr::print(_value.toString());
    }
};

class BinaryOpExpr : public Expr
{
    QString _op;
    Expr * _lhs;
    Expr * _rhs;

public:
    BinaryOpExpr(Expr * lhs, Match op, Expr * rhs)
        : Expr(op, "OP")
    {
        _lhs = lhs;
        _rhs = rhs;
        _op = op.text();
    }

    void print()
    {
        _lhs->print();
        printf(" %s ", qPrintable(_op));
        _rhs->print();
    }
};


class ExpressionExpr : public Expr
{
    Expr * _term;
    QList<QString> _ops;
    QList<Expr *> _terms;

public:
    ExpressionExpr(Expr * term)
        : Expr(Match(), "EXPR")
    {
        _term = term;
    }

    void add(QString op, Expr * term)
    {
        _ops.append(op);
        _terms.append(term);
    }

    void print()
    {
        _term->print();
        for (int i = 0; i < _terms.size(); i++)
        {
            printf(" %s ",qPrintable(_ops[i]));
            _terms[i]->print();
        }
    }

    QVariant value()
    {

        if (_terms.isEmpty())
            return _term->value();

        quint32 v1 = _term->value().toInt();
        printf("%i", v1);

        for (int i = 0; i < _terms.size(); i++)
        {
            quint32 v2 = _terms[i]->value().toInt();
            printf(" %s %i", qPrintable(_ops[i]), v2);

            if (_ops[i] == "\\+")
                v1 = v1 + v2;
            else if (_ops[i] == "-")
                v1 = v1 - v2;
            else if (_ops[i] == "\\*")
                v1 = v1 * v2;
            else if (_ops[i] == "/")
                v1 = v1 / v2;
            else if (_ops[i] == "//")
                v1 = v1 % v2;

            // shift operators
            else if (_ops[i] == ">>")
                v1 = v1 >> v2;
            else if (_ops[i] == "~>")
                v1 = ((qint32) v1) >> v2;
            else if (_ops[i] == "<<")
                v1 = v1 << v2;

            // bitwise operators
            else if (_ops[i] == "&")
                v1 = v1 & v2;
            else if (_ops[i] == "\\|")
                v1 = v1 | v2;
            else if (_ops[i] == "\\^")
                v1 = v1 ^ v2;

            // relational operators
            else if (_ops[i] == ">")
                v1 = (v1 > v2 ? -1 : 0);
            else if (_ops[i] == "<")
                v1 = (v1 < v2 ? -1 : 0);
            else if (_ops[i] == ">=")
                v1 = (v1 >= v2 ? -1 : 0);
            else if (_ops[i] == "<=")
                v1 = (v1 <= v2 ? -1 : 0);

            // equality operators
            else if (_ops[i] == "==")
                v1 = (v1 == v2 ? -1 : 0);
            else if (_ops[i] == "!=")
                v1 = (v1 != v2 ? -1 : 0);



        }

        return QVariant(v1);
    }
};




class FactorExpr : public Expr
{
    Expr * _expr;

public:
    FactorExpr(Expr * expr)
        : Expr(Match(), "FACTOR")
    {
        _expr = expr;
    }

    void print()
    {
        printf("(");
        _expr->print();
        printf(")");
        fflush(stdout);
    }

    QVariant value()
    {
        return _expr->value();
    }
};


*/
