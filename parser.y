%{
#include "types.h"
#include "stdio.h"

int yylex();
void yyerror(const char *s);
void print(const char * s);

extern const char * yytext;
extern int yylineno;
extern FILE *yyin;
extern FILE *yyout;

%}

%start program

%union {
    quint32     num;
    QString     *str;
}

// numbers

%token <str>    STRING
%token <str>    IDENT
%token <num>    BIN
%token <num>    QUAT 
%token <num>    HEX 
%token <num>    DEC

// operators

%token HASH


%%

program : literal 
        | obj_line
        ;

literal         : '#' primary_expr
                ;

obj_line        : IDENT ':' STRING
{
    printf("%s : \"%s\"", qPrintable(*$1), qPrintable(*$3));
}

primary_expr    : number
                | ident

number          : dec
	            | bin
	            | quat
	            | hex
	            ;

dec             : DEC   { printf("%i", $1); }
bin             : BIN   { printf("BIN %%%s", qPrintable(QString::number($1, 2))); }
quat            : QUAT  { printf("QUAT %%%%%s", qPrintable(QString::number($1, 4))); }
hex             : HEX   { printf("HEX $%s", qPrintable(QString::number($1, 16))); }

ident           : IDENT { printf("IDENT %s", qPrintable(*$1)); }

%%


void yyerror(const char *s)
{
	printf("\n\nError(%i): %s\n", yylineno, s);
	exit(-1);
}

void print(const char * s)
{
    printf("%s '%s' ", s, yytext);
}


int main( int argc, char **argv )
{
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yyparse();

    printf("\n");
    printf("\n");
}
