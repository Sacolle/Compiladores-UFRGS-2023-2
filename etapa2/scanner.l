%top{
	// definir este macro no topo para retinar o warning da declaração implicita da função fileno
	#define _POSIX_C_SOURCE 200809L
}

%option noinput
%option nounput

%{
	/*
	* Nome: Pedro Henrique Boniatti Colle
	* Matrícula: 00333916
	*/
	
	//na definição do trabalho tem q realizar esses defines, mas para quê?
	#define SYMBOL_LIT_INTE 1
	#define SYMBOL_LIT_REAL 2
	#define SYMBOL_LIT_CARA 3
	#define SYMBOL_LIT_STRING 4
	#define SYMBOL_IDENTIFIER 7

	#include "hashtable.h"
	#include "y.tab.h"
	

	HashTable* g_table;
	#define INSERT(k, v) insert_table(g_table, k, v)

	int RUNNING = 1;
	int LINENUMBER = 1;
%}

%x COMENTARIO

D [0-9]
C [0-9a-zA-Z_]
BAR [tnr]

%%
char { return KW_CHAR; }
int { return KW_INT; }
float { return KW_FLOAT; }

code { return KW_CODE; }
if { return KW_IF; }
else { return KW_ELSE; }
while { return KW_WHILE; }
input { return KW_INPUT; }
print { return KW_PRINT; }
return { return KW_RETURN; }


[-,;()\[\]{}=+*/%<>&|~] { return yytext[0]; }
"<=" { return OPERATOR_LE; }
">=" { return OPERATOR_GE; }
"==" { return OPERATOR_EQ; }
"!=" { return OPERATOR_DIF; }

{D}+ { 
	yylval.symbol = INSERT(yytext, SYMBOL_LIT_INTE); 
	return LIT_INT;
}
{D}+\.{D}+ { 
	yylval.symbol = INSERT(yytext, SYMBOL_LIT_REAL); 
	return LIT_REAL;
}
'(\\.|.)' { 
	yylval.symbol = INSERT(yytext, SYMBOL_LIT_CARA); 
	return LIT_CHAR;
}
\"(\\.|[^"])*\" { 
	yylval.symbol = INSERT(yytext, SYMBOL_LIT_STRING); 
	return LIT_STRING;
}

{C}+ { 
	yylval.symbol = INSERT(yytext, SYMBOL_IDENTIFIER); 
	return TK_IDENTIFIER; 
}

\n { LINENUMBER++; }
[ \t]

\/\/\/ { BEGIN(COMENTARIO); }
\/\/[^\/\n].*
\/\/

. { return TOKEN_ERROR; }

<COMENTARIO>\\\\\\ { BEGIN(INITIAL); }
<COMENTARIO>.
<COMENTARIO>\n { LINENUMBER++; }

%%

void initMe(void){
	g_table = create_table(997);
}

int getLineNumber(void){
	return LINENUMBER;
}

int isRunning(void){
	return RUNNING;
}

int yywrap(){
	RUNNING = 0;
	return 1;
}
