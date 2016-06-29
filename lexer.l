%option noyywrap
%option case-insensitive
%option yylineno

%x INSTRING INESCAPE INLINECOMMENT INMULTICOMMENT INDOCLINECOMMENT INDOCMULTICOMMENT
%x INDEC INHEX INQUAT INBIN

%{

#include "types.h"
#include "parser.hpp"

void print(const char * s);

QString str_collector;

%}

BIN         [0-1]([0-1_]+[0-1]|[0-1]*)
QUAT        [0-3]([0-3_]+[0-3]|[0-3]*)
DEC         [0-9]([0-9_]+[0-9]|[0-9]*)
HEX         [0-9a-f]([0-9a-f_]+[0-9a-f]|[0-9a-f]*)
FLOAT       {DEC}"."{DEC}

IDENT       [a-z][a-z0-9]*

BOOLEAN     and|or|xor|not
BITWISE     "&"|"|"|"^"
EQUALITY    "=="|"<>"
RELATIONAL  ">"|">="|"<"|"<="
SHIFT       ">>"|"<<"|"~>"|"->"|"<-"|"><"
ARITHMETIC  "+"|"-"|"*"|"/"|"//"
ASSIGN      "="|":="|"+="|"-="|"*="|"/="|"//="|"&="|"|="|"^="
UNARY       "--"|"++"|"?"|"@"|"@@"|"^^"|"||"|"!"|"~~"|"~"

OP          {UNARY}|{ARITHMETIC}|{SHIFT}|{RELATIONAL}|{EQUALITY}|{BITWISE}|{BOOLEAN}|{ASSIGN}

%%

            /* TOKENS */

<INITIAL>"$"        {   BEGIN(INHEX);   }
<INITIAL>"%"        {   BEGIN(INBIN);   }
<INITIAL>"%%"       {   BEGIN(INQUAT);  }

<INBIN>{BIN} {
    yylval.num = QByteArray(yytext).toUInt(0, 2);
    BEGIN(INITIAL);
    return BIN;
}
<INBIN>.    {
    printf( "Not a valid binary number");
    return -1;
}

<INQUAT>{QUAT} {
    yylval.num = QByteArray(yytext).toUInt(0, 4);
    BEGIN(INITIAL);
    return QUAT;
}
<INQUAT>.    {
    printf( "Not a valid quaternary number");
    return -1;
}

<INHEX>{HEX} {
    yylval.num = QByteArray(yytext).toUInt(0, 16);
    BEGIN(INITIAL);
    return HEX;
}
<INHEX>.    {
    printf( "Not a valid hexadecimal number");
    return -1;
}

{DEC} {
    yylval.num = QString(yytext).toInt();
    return DEC;
}

{FLOAT} {
    print("FLOAT");
    return 1;
}

con|var|obj|pub|pri|dat|asm {
    print("BLOCK");
    return 1;
}

if|then|begin|end|procedure|function {
    print("KEYWORD");
    return 1;
}

{IDENT}     {
    yylval.str = new QString(yytext);
    return IDENT;
}

{OP} {
    print("OP");
    return 1;
}

<INITIAL>"'"        {   BEGIN(INLINECOMMENT);       }
<INITIAL>"''"       {   BEGIN(INMULTICOMMENT);      }
<INITIAL>"{"        {   BEGIN(INDOCLINECOMMENT);    }
<INITIAL>"{{"       {   BEGIN(INDOCMULTICOMMENT);   }

<INLINECOMMENT>"\n"     {   BEGIN(INITIAL); }
<INMULTICOMMENT>"\n"    {   BEGIN(INITIAL); }
<INDOCLINECOMMENT>"}}"  {   BEGIN(INITIAL); }
<INDOCMULTICOMMENT>"}}" {   BEGIN(INITIAL); }

<INLINECOMMENT,INMULTICOMMENT,INDOCLINECOMMENT,INDOCMULTICOMMENT>.

<INITIAL>["]        {   
    str_collector = "";
    BEGIN(INSTRING);
}
<INSTRING>["]       {
    yylval.str = new QString(str_collector);
    BEGIN(INITIAL);
    return STRING;
}
<INSTRING>[^"]      {
    str_collector += yytext;
}

[ \t]+          /* eat up whitespace */

^[ ]*\n       {/* Ignore blank lines. */}


\n  {
}

.   return *yytext;

%%


