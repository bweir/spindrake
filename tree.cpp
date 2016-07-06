#include "tree.h"

Expr * foldConstants(Expr * exp)
{
    exp->fold();
    if (!exp->isConstant()) return exp;
    quint32 v = exp->value();
    Expr * newexp = new NumberExpr(10, v);

    if (newexp != NULL)
    {
        delete exp;
        return newexp;
    }
    else
    {
        return exp;
    }
}

void deleteHash(QHash<QString, Expr *> & hash)
{
    foreach (QString i, hash.keys())
    {
        delete hash[i];
    }
}

