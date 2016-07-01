
%{
#include "types.h"
#include "stdio.h"
#include "config.h"

%}

%union {
    quint32     num;
    QString     *str;
}

%{
int  yylex (YYSTYPE*, YYLTYPE*);
int yyerror (YYLTYPE *locp, char const *msg);
void print(const char * s);

extern const char * yytext;
extern int yylineno;
extern FILE *yyin;
extern FILE *yyout;

QString filename;

%}

%locations
%define api.pure full
%define parse.lac full
%define parse.error verbose


%start program


// numbers

%token <str>    OBJSTRING       "object name"
%token <str>    STRING          "string"
%token <str>    IDENT           "identifier"
%token <num>    BIN             "binary number"
%token <num>    QUAT            "quaternary number"
%token <num>    HEX             "hexadecimal number"
%token <num>    DEC             "decimal number"

%type <num>     expr, primary_expr, number

// operators

%token  NL      "newline"
%token  HASH    "hash (#)"

%token  CON     "CON block"
%token  VAR     "VAR block"
%token  OBJ     "OBJ block"
%token  PUB     "PUB block"
%token  PRI     "PRI block"
%token  DAT     "DAT block"
%token  ASM     "ASM block"

%type <str>     ident
%type <num>     bin, quat, dec, hex


%%

program         : blocklist
                ;

blocklist       : block
                | blocklist block
                ;

block           : con con_lines  { printf("\n"); }
                | var var_lines  { printf("\n"); }
                | obj obj_lines  { printf("\n"); }
                ;


var_lines       : ;
pub_lines       : ;
pri_lines       : ;
dat_lines       : ;

con             : CON NL    { printf("CON\n"); }
con_lines       : con_line
                | con_lines con_line

con_line        : IDENT '=' expr NL
                {
                    printf("%s = %u\n", qPrintable(*$1), $3);
                }
                | literal ',' con_array NL

con_array       : con_array_item
                | con_array_item ',' con_array

con_array_item  : ident
                | ident array_index


var             : VAR NL    { printf("VAR\n"); }



obj             : OBJ NL    { printf("OBJ\n"); }

obj_lines       : obj_line
                | obj_lines obj_line

obj_line        : ident ':' OBJSTRING NL
                {
                    printf("%s : \"%s\"\n", qPrintable(*$1), qPrintable(*$3));
                }
                | ident array_index ':' OBJSTRING NL
                {
                    printf("%s : \"%s\"\n", qPrintable(*$1), qPrintable(*$4));
                }
                ;


pub             : PUB NL    { printf("PUB\n"); }
pri             : PRI NL    { printf("PRI\n"); }
dat             : DAT NL    { printf("DAT\n"); }

literal         : '#' expr
array_index     : '[' expr ']'

expr            : primary_expr
                | '(' expr ')'

primary_expr    : number

number          : dec
	            | bin
	            | quat
	            | hex
	            ;



dec             : DEC   { printf("%i ", $1); }
bin             : BIN   { printf("BIN %%%s", qPrintable(QString::number($1, 2))); }
quat            : QUAT  { printf("QUAT %%%%%s", qPrintable(QString::number($1, 4))); }
hex             : HEX   { printf("HEX $%s", qPrintable(QString::number($1, 16))); }

ident           : IDENT { $$ = $1; printf("IDENT %s", qPrintable(*$1)); }

%%


int yyerror (YYLTYPE *locp, char const *msg)
{
    fflush(stdout);
    fflush(stderr);
	fprintf(stderr, "\n\033[1;37m%s(%i,%i) \033[1;31merror:\033[0m %s\n\n", qPrintable(filename), locp->first_line, locp->first_column, msg);
    fprintf(stderr, "%s\n", qPrintable(locp->line));
    fprintf(stderr, "%s", qPrintable(QString(locp->first_column - 1, ' ')));
    fprintf(stderr, "\033[1;37m%s\033[0m\n", qPrintable(QString(locp->last_column - locp->first_column, '-')));
    fflush(stderr);
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

    filename = argv[0];

    yyparse();

    printf("\n");
    printf("\n");
}
