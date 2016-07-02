%option noyywrap
%option case-insensitive
%option yylineno
%option bison-locations

%x INSTRING INOBJSTRING INESCAPE INLINECOMMENT INMULTICOMMENT INDOCLINECOMMENT INDOCMULTICOMMENT
%x INDEC INHEX INQUAT INBIN

%{

#include "types.h"
#include "parser.hpp"
#include "config.h"

int colnum = 1;

#define YY_USER_ACTION {\
    yylloc->line += yytext;         \
    yylloc->first_line = yylineno;  \
    yylloc->first_column = colnum;  \
    colnum = colnum + yyleng;       \
    yylloc->last_column = colnum;   \
    yylloc->last_line = yylineno;   \
}

extern int yyerror (YYLTYPE *locp, char const *msg);

#define ERROR(msg) yyerror(yylloc, msg)

void print(const char * s);

enum Block {
    NoBlock,
    ConBlock,
    VarBlock,
    ObjBlock,
    PubBlock,
    PriBlock,
    DatBlock,
    AsmBlock
};

Block block = NoBlock;
bool startingline = true;

QString str_collector;

%}

BIN         [0-1]([0-1_]+[0-1]|[0-1]*)
QUAT        [0-3]([0-3_]+[0-3]|[0-3]*)
DEC         [0-9]([0-9_]+[0-9]|[0-9]*)
HEX         [0-9a-f]([0-9a-f_]+[0-9a-f]|[0-9a-f]*)
FLOAT       {DEC}"."{DEC}

IDENT       [a-z][a-z0-9]*

BITWISE     "&"|"|"|"^"
EQUALITY    "=="|"<>"
RELATIONAL  ">"|">="|"<"|"<="
SHIFT       ">>"|"<<"|"~>"|"->"|"<-"|"><"
ARITHMETIC  "+"|"-"|"*"|"/"|"//"
ASSIGN      ":="|"+="|"-="|"*="|"/="|"//="|"&="|"|="|"^="
UNARY       "--"|"++"|"?"|"@"|"@@"|"^^"|"||"|"!"|"~~"|"~"

OP          {UNARY}|{ARITHMETIC}|{SHIFT}|{RELATIONAL}|{EQUALITY}|{BITWISE}|{ASSIGN}

%%

^[ ]*\n     /* Ignore blank lines. */

[ \t]* {
    if (startingline && yylloc->first_column == 1)
    {
        printf("%s", qPrintable(QString(yyleng, ' ')));
    }
    startingline  = false;
}

<INITIAL>"'"        {   BEGIN(INLINECOMMENT);       }
<INITIAL>"''"       {   BEGIN(INMULTICOMMENT);      }
<INITIAL>"{"        {   BEGIN(INDOCLINECOMMENT);    }
<INITIAL>"{{"       {   BEGIN(INDOCMULTICOMMENT);   }

<INLINECOMMENT>"\n"     {   BEGIN(INITIAL); }
<INMULTICOMMENT>"\n"    {   BEGIN(INITIAL); }
<INDOCLINECOMMENT>"}"   {   BEGIN(INITIAL); }
<INDOCMULTICOMMENT>"}}" {   BEGIN(INITIAL); }

<INLINECOMMENT,INMULTICOMMENT,INDOCLINECOMMENT,INDOCMULTICOMMENT>.

<INITIAL,INLINECOMMENT,INMULTICOMMENT>\n {
    colnum = 1;
    yylloc->line = "";
    startingline = true;
    return NL;
}

<INITIAL>["]        {   
    if (block == ObjBlock)
    {
        BEGIN(INOBJSTRING);
    }
    else
    {
        str_collector = "";
        BEGIN(INSTRING);
    }
}

<INOBJSTRING>["] {
    BEGIN(INITIAL);
}

<INOBJSTRING>[_a-zA-Z0-9.\- ]+ {
    yylval->str = new QString(yytext);
    return OBJSTRING;
}

<INSTRING>["] {
    yylval->str = new QString(str_collector);
    BEGIN(INITIAL);
    return STRING;
}

<INSTRING>[^"] {
    str_collector += yytext;
}

            /* TOKENS */

<INITIAL>"$"  { BEGIN(INHEX);  }
<INITIAL>"%"  { BEGIN(INBIN);  }
<INITIAL>"%%" { BEGIN(INQUAT); }

<INBIN>{BIN} {
    yylval->num = QByteArray(yytext).toUInt(0, 2);
    BEGIN(INITIAL);
    return BINARY;
}
<INBIN>.    {
    ERROR("Not a valid binary number (0-1)");
    return -1;
}

<INQUAT>{QUAT} {
    yylval->num = QByteArray(yytext).toUInt(0, 4);
    BEGIN(INITIAL);
    return QUATERNARY;
}
<INQUAT>.    {
    ERROR("Not a valid quaternary number (0-3)");
    return -1;
}

<INHEX>{HEX} {
    yylval->num = QByteArray(yytext).toUInt(0, 16);
    BEGIN(INITIAL);
    return HEXADECIMAL;
}
<INHEX>.    {
    ERROR("Not a valid hexadecimal number (0-9a-f)");
    return -1;
}

{DEC} {
    yylval->num = QString(yytext).toInt();
    return DECIMAL;
}

{FLOAT} {
    yylval->fl = QString(yytext).toFloat();
    return FLOAT;
}

"+="    return ADD_ASSIGN;
"-="    return SUB_ASSIGN;
"*="    return MUL_ASSIGN;
"//="   return MOD_ASSIGN;
"/="    return DIV_ASSIGN;
"<<="   return SHL_ASSIGN;
">>="   return SHR_ASSIGN;
"~>="   return SAR_ASSIGN;
"<-="   return ROL_ASSIGN;
"->="   return ROR_ASSIGN;
"><="   return REV_ASSIGN;
"&="    return AND_ASSIGN;
"|="    return OR_ASSIGN;
"^="    return XOR_ASSIGN;

"--"    return DEC;
"++"    return INC;
"~~"    return SET;
"~"     return CLEAR;

"<<"    return SHL;
">>"    return SHR;
"~>"    return SAR;
"<-"    return ROL;
"->"    return ROR;
"><"    return REV;

"<="    return LESSEQ;
">="    return GREATEREQ;
"=="    return EQ;
"<>"    return NEQ;
"<"     return LESS;
">"     return GREATER;

"+"     return PLUS;
"-"     return MINUS;
"*"     return MUL;
"//"    return MOD;
"/"     return DIV;

"["     return BRAC_L;
"]"     return BRAC_R;
"("     return PAREN_L;
")"     return PAREN_R;

"@"     return ADDR;
"#"     return LITERAL;
":"     return ALIAS;
"="     return ASSIGN;
","     return COMMA;

"!"     return BW_NOT;
"&"     return BW_AND;
"|"     return BW_OR;
"^"     return BW_XOR;

not     return BOOL_NOT;
and     return BOOL_AND;
or      return BOOL_OR;

byte    return BYTE;
word    return WORD;
long    return LONG;

con     { block = ConBlock; return CON; }
var     { block = VarBlock; return VAR; }
obj     { block = ObjBlock; return OBJ; }
pub     { block = PubBlock; return PUB; }
pri     { block = PriBlock; return PRI; }
dat     { block = DatBlock; return DAT; }
asm     { block = AsmBlock; return ASM; }

{IDENT}     {
    yylval->str = new QString(yytext);
    return IDENT;
}

. {
    ERROR("unrecognized character!");
}

%%


