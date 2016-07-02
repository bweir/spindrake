
%{
#include "types.h"
#include "stdio.h"
#include "config.h"

%}

%union {
    quint32     num;
    float       fl;
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
%token <num>    BINARY          "binary number"
%token <num>    QUATERNARY      "quaternary number"
%token <num>    HEXADECIMAL     "hexadecimal number"
%token <num>    DECIMAL         "decimal number"
%token <fl>     FLOAT

%type <num>     expr primary_expr number
%type <num>     add_expr mult_expr factor array_index

%type <str>     ident
%type <num>     bin quat dec hex

// operators

%token  NL      "new line"

%token  ADDR    "address (@)"
%token  LITERAL "literal (#)"
%token  ALIAS   "alias (:)"
%token  ASSIGN  "assignment (=)"
%token  COMMA   "comma (,)"

%token  PLUS    "add (+)"
%token  MINUS   "subtract (-)"
%token  MUL     "multiply (*)"
%token  DIV     "divide (/)"
%token  MOD     "modulo (//)"

%token  SHL     "shift left (<<)"
%token  SHR     "shift right (>>)"
%token  SAR     "signed shift right (~>)"
%token  ROL     "rotate left (<-)"
%token  ROR     "rotate right (->)"
%token  REV     "reverse (><)"

%token  LESS        "less than (<)"
%token  GREATER     "greater than (>)"
%token  LESSEQ      "less than or equal(<=)"
%token  GREATEREQ   "greater than or equal(>=)"
%token  EQ          "equal (==)"
%token  NEQ         "not equal (<>)"

%token  DEC     "decrement (--)"
%token  INC     "increment (++)"
%token  CLEAR   "clear (~)"
%token  SET     "set (~~)"

%token  PAREN_L "open parenthesis (()"
%token  PAREN_R "close parenthesis ())"
%token  BRAC_L  "open bracket ([)"
%token  BRAC_R  "close bracket (])"

%token  BW_NOT  "bitwise NOT (!)"
%token  BW_AND  "bitwise AND (&)"
%token  BW_OR   "bitwise OR (|)"
%token  BW_XOR  "bitwise XOR (^)"

%token  BOOL_NOT  "boolean NOT"
%token  BOOL_AND  "boolean AND"
%token  BOOL_OR   "boolean OR"

%token  CON     "CON block"
%token  VAR     "VAR block"
%token  OBJ     "OBJ block"
%token  PUB     "PUB block"
%token  PRI     "PRI block"
%token  DAT     "DAT block"
%token  ASM     "ASM block"

%token  BYTE    "byte value"
%token  WORD    "word value"
%token  LONG    "long value"

%token ADD_ASSIGN   "add assign (+=)"
%token SUB_ASSIGN   "subtract assign (-=)"
%token MUL_ASSIGN   "multiply assign (*=)"
%token MOD_ASSIGN   "modulo assign (//=)"
%token DIV_ASSIGN   "divide assign (/=)"
%token SHL_ASSIGN   "shift left assign (<<=)"
%token SHR_ASSIGN   "shift right assign (>>=)"
%token SAR_ASSIGN   "signed shift right assign (~>=)"
%token ROL_ASSIGN   "rotate left assign (<-=)"
%token ROR_ASSIGN   "rotate right assign (->=)"
%token REV_ASSIGN   "reverse assign (><=)"
%token AND_ASSIGN   "bitwise and assign (&=)"
%token OR_ASSIGN    "bitwise or assign (|=)"
%token XOR_ASSIGN   "bitwise xor assign (^=)"



%%

program         : blocklist
                ;

blocklist       : block
                | blocklist block
                ;

block           : con con_lines  { printf("\n"); }
                | var var_lines  { printf("\n"); }
                | obj obj_lines  { printf("\n"); }
                | dat dat_lines  { printf("\n"); }
                ;

// con blocks
// -----------------------------------------------------

con             : CON NL    { printf("CON\n"); }
                ;

con_lines       : con_line
                | con_lines con_line
                ;

con_line        : IDENT ASSIGN expr NL
                {
                    printf("%s = %u\n", qPrintable(*$1), $3);
                }
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

var             : VAR NL    { printf("VAR\n"); }
                ;

var_lines       : var_line
                | var_lines var_line
                ;

var_line        : data_type IDENT NL
                | data_type IDENT array_index NL
                ;

// obj blocks
// -----------------------------------------------------

obj             : OBJ NL    { printf("OBJ\n"); }
                ;

obj_lines       : obj_line
                | obj_lines obj_line
                ;

obj_line        : ident ALIAS OBJSTRING NL
                {
                    printf("%s : \"%s\"\n", qPrintable(*$1), qPrintable(*$3));
                }
                | ident array_index ALIAS OBJSTRING NL
                {
                    printf("%s : \"%s\"\n", qPrintable(*$1), qPrintable(*$4));
                }
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

dat             : DAT NL    { printf("DAT\n"); }
                ;

dat_lines       : dat_line
                | dat_lines dat_line
                ;

dat_line        : ident NL
                | dat_entry NL
                | ident dat_entry NL
                ;

dat_entry       : data_type data_items
                | data_type data_type data_items
                ;

data_items      : data_item
                | data_items COMMA data_item
                ;

data_item       : expr
                ;

// expression parsing
// -----------------------------------------------------

literal         : LITERAL expr
                ;

array_index     : BRAC_L expr BRAC_R  { $$ = $2; }
                ;

expr            : assign_expr
                ;

assign_expr     : bool_or_expr
                | assign_expr ASSIGN     bool_or_expr
                | assign_expr ADD_ASSIGN bool_or_expr
                | assign_expr SUB_ASSIGN bool_or_expr
                | assign_expr MUL_ASSIGN bool_or_expr
                | assign_expr MOD_ASSIGN bool_or_expr
                | assign_expr DIV_ASSIGN bool_or_expr
                | assign_expr SHL_ASSIGN bool_or_expr
                | assign_expr SHR_ASSIGN bool_or_expr
                | assign_expr SAR_ASSIGN bool_or_expr
                | assign_expr ROL_ASSIGN bool_or_expr
                | assign_expr ROR_ASSIGN bool_or_expr
                | assign_expr REV_ASSIGN bool_or_expr
                | assign_expr AND_ASSIGN bool_or_expr
                | assign_expr OR_ASSIGN  bool_or_expr
                | assign_expr XOR_ASSIGN bool_or_expr


bool_or_expr    : bool_and_expr
                | BOOL_OR bool_and_expr 
                ;

bool_and_expr   : bool_not_expr
                | BOOL_AND bool_not_expr 
                ;

bool_not_expr   : relation_expr
                | BOOL_NOT relation_expr
                ;

relation_expr   : add_expr
                | relation_expr EQ        add_expr
                | relation_expr NEQ       add_expr
                | relation_expr LESS      add_expr
                | relation_expr GREATER   add_expr
                | relation_expr LESSEQ    add_expr
                | relation_expr GREATEREQ add_expr
                ;

add_expr        : mult_expr
                | add_expr PLUS  mult_expr
                | add_expr MINUS mult_expr
                ;

mult_expr       : bw_or_expr
                | mult_expr MUL bw_or_expr
                | mult_expr MOD bw_or_expr
                | mult_expr DIV bw_or_expr
                ;

bw_or_expr      : bw_and_expr
                | bw_or_expr BW_OR  bw_and_expr
                | bw_or_expr BW_XOR bw_and_expr
                ;

bw_and_expr     : shift_expr
                | bw_and_expr BW_AND shift_expr
                ;

shift_expr      : unary_expr
                | shift_expr SHL unary_expr
                | shift_expr SHR unary_expr
                | shift_expr SAR unary_expr
                | shift_expr ROL unary_expr
                | shift_expr ROR unary_expr
                | shift_expr REV unary_expr
                ;

unary_expr      : unary2_expr
                | MINUS  unary2_expr
                | BW_NOT unary2_expr
                ;

unary2_expr     : factor
                | DEC   factor
                | INC   factor
                | SET   factor
                | CLEAR factor
                | factor DEC
                | factor INC
                | factor SET
                | factor CLEAR
                ;

factor          : primary_expr
                | PAREN_L expr PAREN_R
                ;

primary_expr    : number
                | address
                ;

address         : ADDR ident
                | ident array_index
                ;


// -----------------------------------------------------

data_type       : BYTE
                | WORD
                | LONG
                ;

number          : dec
	            | bin
	            | quat
	            | hex
	            ;

dec             : DECIMAL { printf("%i ", $1); }
                ;

bin             : BINARY { printf("BIN %%%s", qPrintable(QString::number($1, 2))); }
                ;

quat            : QUATERNARY { printf("QUAT %%%%%s", qPrintable(QString::number($1, 4))); }
                ;

hex             : HEXADECIMAL { printf("HEX $%s", qPrintable(QString::number($1, 16))); }
                ;

ident           : IDENT { $$ = $1; printf("IDENT %s", qPrintable(*$1)); }
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


int main( int argc, char **argv )
{
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    filename = argv[0];

    yyparse();
}
