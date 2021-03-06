
%{
#include "types.h"
#include "parser.hpp"
#include "tree.h"

ObjectExpr * rootExpr = NULL;
%}

%union {
    quint32         num;
    float           fl;
    const char      *str;
    Expr            *exp;
    QList<Expr *>   *list;
}

%{
int yylex (YYSTYPE*, YYLTYPE*);
int yyerror (YYLTYPE *locp, char const *msg);
void print(const char * s);

extern const char * yytext;
extern int yylineno;
extern FILE *yyin;
extern FILE *yyout;

QString filename;

%}

%token-table
%locations
%define api.pure full
%define parse.lac full
%define parse.error verbose

%start program


// numbers

%token <str>    OBJSTRING       "object name"
%token <str>    STRING          "string"
%token <str>    IDENT           "identifier"
%token <num>    NUMBER          "number"
%token <num>    BINARY          "binary number"
%token <num>    QUATERNARY      "quaternary number"
%token <num>    HEXADECIMAL     "hexadecimal number"
%token <num>    DECIMAL         "decimal number"
%token <fl>     FLOAT

%type <exp>     expr primary_expr 

%type <exp>     assign_expr
%type <exp>     bool_or_expr
%type <exp>     bool_and_expr
%type <exp>     bool_not_expr
%type <exp>     relation_expr
%type <exp>     add_expr
%type <exp>     mult_expr
%type <exp>     bw_or_expr
%type <exp>     bw_and_expr
%type <exp>     shift_expr
%type <exp>     unary_expr
%type <exp>     unary2_expr
%type <exp>     literal address factor
%type <exp>     array_index
%type <exp>     ident bin quat dec hex number
%type <exp>     data_type

%type <list>    blocklist
%type <exp>     block

%type <exp>     con
%type <list>    con_lines
%type <exp>     con_line

%type <exp>     dat 
%type <list>    dat_lines
%type <exp>     dat_line
%type <list>    dat_items
%type <exp>     dat_item
%type <exp>     dat_symbol dat_align dat_size


// operators

%token  NL          "new line"

%token  ADDR        "address operator (@)"
%token  LITERAL     "literal operator (#)"
%token  ALIAS       "alias operator (:)"
%token  ASSIGN      "assignment operator (=)"
%token  COMMA       "comma operator (,)"

%token  PLUS        "add operator (+)"
%token  MINUS       "subtract operator (-)"
%token  MUL         "multiply operator (*)"
%token  DIV         "divide operator (/)"
%token  MOD         "modulo operator (//)"

%token  SHL         "shift left operator (<<)"
%token  SHR         "shift right operator (>>)"
%token  SAR         "signed shift right operator (~>)"
%token  ROL         "rotate left operator (<-)"
%token  ROR         "rotate right operator (->)"
%token  REV         "reverse operator (><)"

%token  LESS        "less than operator (<)"
%token  GREATER     "greater than operator (>)"
%token  LESSEQ      "less than or equal operator (<=)"
%token  GREATEREQ   "greater than or equal operator (>=)"
%token  EQ          "equal operator (==)"
%token  NEQ         "not equal operator (<>)"

%token  DEC         "decrement operator (--)"
%token  INC         "increment operator (++)"
%token  CLEAR       "clear operator (~)"
%token  SET         "set operator (~~)"

%token  PAREN_L     "open parenthesis (()"
%token  PAREN_R     "close parenthesis ())"
%token  BRAC_L      "open bracket ([)"
%token  BRAC_R      "close bracket (])"

%token  BW_NOT      "bitwise NOT operator (!)"
%token  BW_AND      "bitwise AND operator (&)"
%token  BW_OR       "bitwise OR operator (|)"
%token  BW_XOR      "bitwise XOR operator (^)"

%token  BOOL_NOT    "boolean NOT"
%token  BOOL_AND    "boolean AND"
%token  BOOL_OR     "boolean OR"

%token  CON         "CON block"
%token  VAR         "VAR block"
%token  OBJ         "OBJ block"
%token  PUB         "PUB block"
%token  PRI         "PRI block"
%token  DAT         "DAT block"
%token  ASM         "ASM block"

%token  BYTE        "byte"
%token  WORD        "word"
%token  LONG        "long"

%token ADD_ASSIGN   "add assignment operator (+=)"
%token SUB_ASSIGN   "subtract assignment operator (-=)"
%token MUL_ASSIGN   "multiply assignment operator (*=)"
%token MOD_ASSIGN   "modulo assignment operator (//=)"
%token DIV_ASSIGN   "divide assignment operator (/=)"
%token SHL_ASSIGN   "shift left assignment operator (<<=)"
%token SHR_ASSIGN   "shift right assignment operator (>>=)"
%token SAR_ASSIGN   "signed shift right assignment operator (~>=)"
%token ROL_ASSIGN   "rotate left assignment operator (<-=)"
%token ROR_ASSIGN   "rotate right assignment operator (->=)"
%token REV_ASSIGN   "reverse assignment operator (><=)"
%token AND_ASSIGN   "bitwise and assignment operator (&=)"
%token OR_ASSIGN    "bitwise or assignment operator (|=)"
%token XOR_ASSIGN   "bitwise xor assignment operator (^=)"

%token END 0        "end of file"



%%

program         : blocklist                                     { rootExpr = new ObjectExpr(filename, $1); }
                ;

blocklist       : blocklist block                               { $$ = $1; $1->append($2); }
                |                                               { $$ = new QList<Expr *>(); }
                ;

block           : con
                | dat 
                ;

// con blocks
// -----------------------------------------------------

con             : CON NL con_lines                              { $$ = new BlockExpr(ConBlock, $3); }
                ;

con_lines       : con_lines con_line                            { $$ = $1; $1->append($2); }
                |                                               { $$ = new QList<Expr *>(); }
                ;

con_line        : ident ASSIGN expr NL                          { $$ = new ConAssignExpr($1, $3); }
                | literal COMMA con_array NL
                ;

con_array       : con_array_item
                | con_array COMMA con_array_item
                ;

con_array_item  : ident
                | ident array_index
                ;

// var blocks
// -----------------------------------------------------

var             : VAR NL var_lines
                ;

var_lines       : var_line
                | var_lines var_line
                ;

var_line        : data_type IDENT NL
                | data_type IDENT array_index NL
                ;

// obj blocks
// -----------------------------------------------------

obj             : OBJ NL obj_lines
                ;

obj_lines       : obj_line
                | obj_lines obj_line
                ;

obj_line        : obj_alias OBJSTRING NL
                ;

obj_alias       : ident ALIAS
                | ident array_index ALIAS
                ;

// pub/pri blocks
// -----------------------------------------------------

pub             : PUB NL    { printf("PUB\n"); }
                ;

pri             : PRI NL    { printf("PRI\n"); }
                ;

pub_lines       :
                ;

pri_lines       :
                ;

// dat blocks
// -----------------------------------------------------

dat             : DAT NL dat_lines                              { $$ = new BlockExpr(DatBlock, $3); }
                
dat_lines       : dat_lines dat_line                            { $$ = $1; $1->append($2); }
                |                                               { $$ = new QList<Expr *>(); }
                ;

dat_line        : dat_align dat_item dat_items NL               { $3->prepend($2); $$ = new DatLineExpr(new IdentExpr(""), $1, $3); }
                | ident dat_align dat_item dat_items NL         { $4->prepend($3); $$ = new DatLineExpr($1, $2, $4); }
                | ident NL dat_align dat_item dat_items NL      { $5->prepend($4); $$ = new DatLineExpr($1, $3, $5); }
                ;

dat_align       : data_type
                ;

dat_items       : dat_items COMMA dat_item                      { $$ = $1; $$->append($3); }
                |                                               { $$ = new QList<Expr *>(); }
                ;

dat_item        : data_type expr array_index                    { $$ = new DatItemExpr($1,                 $2, $3); }
                | data_type expr                                { $$ = new DatItemExpr($1,                 $2, new NumberExpr(10, 0)); }
                | expr array_index                              { $$ = new DatItemExpr(new DataTypeExpr(), $1, $2); }
                | expr                                          { $$ = new DatItemExpr(new DataTypeExpr(), $1, new NumberExpr(10, 0)); }
                ;

// expression parsing
// -----------------------------------------------------

literal         : LITERAL expr                          { $$ = new LiteralExpr($2); }
                ;

array_index     : BRAC_L expr BRAC_R                    { $$ = new WrapExpr("[", $2, "]"); }
                ;

expr            : assign_expr
                ;

assign_expr     : bool_or_expr
                | assign_expr ASSIGN     bool_or_expr   { $$ = new BinaryExpr($1, "=",   $3); }
                | assign_expr ADD_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "+=",  $3); }
                | assign_expr SUB_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "-=",  $3); }
                | assign_expr MUL_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "*=",  $3); }
                | assign_expr MOD_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "//=", $3); }
                | assign_expr DIV_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "/=",  $3); }
                | assign_expr SHL_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "<<=", $3); }
                | assign_expr SHR_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, ">>=", $3); }
                | assign_expr SAR_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "~>=", $3); }
                | assign_expr ROL_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "<-=", $3); }
                | assign_expr ROR_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "->=", $3); }
                | assign_expr REV_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "><=", $3); }
                | assign_expr AND_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "&=",  $3); }
                | assign_expr OR_ASSIGN  bool_or_expr   { $$ = new BinaryExpr($1, "|=",  $3); }
                | assign_expr XOR_ASSIGN bool_or_expr   { $$ = new BinaryExpr($1, "^=",  $3); }


bool_or_expr    : bool_and_expr
                | bool_or_expr BOOL_OR bool_and_expr    { $$ = new BinaryExpr($1, "or", $3); }
                ;

bool_and_expr   : bool_not_expr
                | bool_and_expr BOOL_AND bool_not_expr  { $$ = new BinaryExpr($1, "and", $3); }
                ;

bool_not_expr   : relation_expr
                | BOOL_NOT relation_expr                { $$ = new UnaryExpr("not", $2); }
                ;

relation_expr   : add_expr
                | relation_expr EQ        add_expr  { $$ = new BinaryExpr($1, "==", $3); }
                | relation_expr NEQ       add_expr  { $$ = new BinaryExpr($1, "<>", $3); }
                | relation_expr LESS      add_expr  { $$ = new BinaryExpr($1, "<",  $3); }
                | relation_expr GREATER   add_expr  { $$ = new BinaryExpr($1, ">",  $3); }
                | relation_expr LESSEQ    add_expr  { $$ = new BinaryExpr($1, "<=", $3); }
                | relation_expr GREATEREQ add_expr  { $$ = new BinaryExpr($1, ">=", $3); }
                ;

add_expr        : mult_expr
                | add_expr PLUS  mult_expr          { $$ = new BinaryExpr($1, "+", $3); }
                | add_expr MINUS mult_expr          { $$ = new BinaryExpr($1, "-", $3); }
                ;

mult_expr       : bw_or_expr
                | mult_expr MUL bw_or_expr          { $$ = new BinaryExpr($1, "*",  $3); }
                | mult_expr MOD bw_or_expr          { $$ = new BinaryExpr($1, "//", $3); }
                | mult_expr DIV bw_or_expr          { $$ = new BinaryExpr($1, "/",  $3); }
                ;

bw_or_expr      : bw_and_expr
                | bw_or_expr BW_OR  bw_and_expr     { $$ = new BinaryExpr($1, "|", $3); }
                | bw_or_expr BW_XOR bw_and_expr     { $$ = new BinaryExpr($1, "^", $3); }
                ;

bw_and_expr     : shift_expr
                | bw_and_expr BW_AND shift_expr     { $$ = new BinaryExpr($1, "&", $3); }
                ;

shift_expr      : unary_expr
                | shift_expr SHL unary_expr         { $$ = new BinaryExpr($1, "<<", $3); }
                | shift_expr SHR unary_expr         { $$ = new BinaryExpr($1, ">>", $3); }
                | shift_expr SAR unary_expr         { $$ = new BinaryExpr($1, "~>", $3); }
                | shift_expr ROL unary_expr         { $$ = new BinaryExpr($1, "<-", $3); }
                | shift_expr ROR unary_expr         { $$ = new BinaryExpr($1, "->", $3); }
                | shift_expr REV unary_expr         { $$ = new BinaryExpr($1, "><", $3); }
                ;

unary_expr      : unary2_expr
                | MINUS  unary2_expr    { $$ = new UnaryExpr("-", $2); }
                | BW_NOT unary2_expr    { $$ = new UnaryExpr("!", $2); }
                ;

unary2_expr     : factor
                | DEC   factor          { $$ = new UnaryExpr("--", $2); }
                | INC   factor          { $$ = new UnaryExpr("++", $2); }
                | SET   factor          { $$ = new UnaryExpr("~~", $2); }
                | CLEAR factor          { $$ = new UnaryExpr("~" , $2); }
                | factor DEC            { $$ = new UnaryExpr($1, "--"); }
                | factor INC            { $$ = new UnaryExpr($1, "++"); }
                | factor SET            { $$ = new UnaryExpr($1, "~~"); }
                | factor CLEAR          { $$ = new UnaryExpr($1, "~" ); }
                ;

factor          : primary_expr
                | PAREN_L expr PAREN_R  { $$ = new WrapExpr("(", $2, ")"); }
                ;

primary_expr    : number
                | address
                | ident
                ;

address         : ADDR ident            { $$ = new AddressExpr($2, new NumberExpr(10, 0)); }
                | ident array_index     { $$ = new AddressExpr($1, $2); }
                ;

// -----------------------------------------------------

data_type       : BYTE                  { $$ = new DataTypeExpr(DataByte); }
                | WORD                  { $$ = new DataTypeExpr(DataWord); }
                | LONG                  { $$ = new DataTypeExpr(DataLong); }
                ;

number          : dec
	            | bin
	            | quat
	            | hex
	            ;

dec             : DECIMAL               { $$ = new NumberExpr(10, $1); }
                ;

bin             : BINARY                { $$ = new NumberExpr(2, $1); }
                ;

quat            : QUATERNARY            { $$ = new NumberExpr(4, $1); }
                ;

hex             : HEXADECIMAL           { $$ = new NumberExpr(16, $1); }
                ;

ident           : IDENT                 { $$ = new IdentExpr($1); }
                ;

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

