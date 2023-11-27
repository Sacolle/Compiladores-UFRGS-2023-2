%{
	/*
	* Nome: Pedro Henrique Boniatti Colle
	* Matrícula: 00333916
	*/

	#include "tokens.h"
	#include "hashtable.h"

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

{D}+ { INSERT(yytext, LIT_INT); return LIT_INT; }
{D}+\.{D}+ { INSERT(yytext, LIT_REAL); return LIT_REAL; }
'(\\.|.)' { INSERT(yytext, LIT_CHAR); return LIT_CHAR; }
\"(\\.|[^"])*\" { INSERT(yytext, LIT_STRING); return LIT_STRING; }

{C}+ { INSERT(yytext, TK_IDENTIFIER); return TK_IDENTIFIER; }

\n { LINENUMBER++; }
[ \t]

\/\/\/.* { BEGIN(COMENTARIO); }
\/\/.*

. { return TOKEN_ERROR; }

<COMENTARIO>\\\\\\ { BEGIN(INITIAL); }
<COMENTARIO>.
<COMENTARIO>\n { LINENUMBER++; }

%%

void initMe(void){
	g_table = create_table(16);
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
