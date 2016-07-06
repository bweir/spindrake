#include "tree.h"
#include "printer.h"
#include "navigator.h"
#include <QDebug>

extern FILE *yyin;
extern FILE *yyout;

extern int yyparse();
extern ObjectExpr * rootExpr;
extern QString filename;

int main( int argc, char **argv )
{
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    filename = argv[0];

    yyparse();

    Printer printer;

//    navigator.walk(rootExpr, printer);
    printer.print(rootExpr);
    rootExpr->fold();
    printer.print(rootExpr);
//

//    foreach(QString i, rootExpr->constants.keys())
//    {
//        printer.print(rootExpr->constants[i]);
//    }

    delete rootExpr;
}
