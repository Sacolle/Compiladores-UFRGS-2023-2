/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include <stdio.h>
#include "lex.yy.h"
#include "tokens.h"
#include "hashtable.h"

int yylex();
extern char *yytext;
extern FILE *yyin;
extern HashTable *g_table; //NOTE: g_table é declarada no scanner.l

int getLineNumber();
int isRunning(void);
void initMe(void);


int main(int argc, char** argv){

	int tok;

	if (argc < 2){
		printf("call: ./etapa1 input.txt\n");
		exit(1);
    }
  	if (0 == (yyin = fopen(argv[1], "r"))){
		printf("Cannot open file %s... \n",argv[1]);
		exit(1);
    }

	initMe();
	while(isRunning()){

		tok = yylex();
		if (!isRunning())
      		break;

		switch(tok){
			case KW_CHAR: fprintf(stderr, "%d - KW_CHAR: %d \n", getLineNumber(), tok); break;
			case KW_INT:  fprintf(stderr, "%d - KW_INT: %d \n", getLineNumber(), tok); break;
   			case KW_FLOAT:fprintf(stderr, "%d - KW_FLOAT: %d \n", getLineNumber(), tok); break;

   			case KW_CODE: fprintf(stderr, "%d - KW_CODE: %d \n", getLineNumber(), tok); break;

   			case KW_IF:   fprintf(stderr,  "%d - KW_IF: %d \n", getLineNumber(), tok); break;
   			case KW_ELSE: fprintf(stderr,  "%d - KW_ELSE: %d \n", getLineNumber(), tok); break;
   			case KW_WHILE: fprintf(stderr, "%d - KW_WHILE: %d \n", getLineNumber(), tok); break;
   			case KW_INPUT: fprintf(stderr, "%d - KW_INPUT: %d \n", getLineNumber(), tok); break;
   			case KW_PRINT: fprintf(stderr, "%d - KW_PRINT: %d \n", getLineNumber(), tok); break;
   			case KW_RETURN: fprintf(stderr,"%d - KW_RETURNok: %d \n", getLineNumber(), tok); break;

   			case OPERATOR_LE: fprintf(stderr, "%d - OPERATOR_LE: %d \n", getLineNumber(), tok); break;
   			case OPERATOR_GE: fprintf(stderr, "%d - OPERATOR_GE: %d \n", getLineNumber(), tok); break;
   			case OPERATOR_EQ: fprintf(stderr, "%d - OPERATOR_EQ: %d \n", getLineNumber(), tok); break;
   			case OPERATOR_DIF: fprintf(stderr,"%d - OPERATOR_DIF: %d \n", getLineNumber(), tok); break;

   			case TK_IDENTIFIER: fprintf(stderr, "%d - TK_IDENTIFIER: %d \n", getLineNumber(), tok); break;
   
   			case LIT_INT: fprintf(stderr, "%d - LIT_INT: %d \n", getLineNumber(), tok); break;
   			case LIT_REAL: fprintf(stderr,"%d - LIT_REAL: %d \n", getLineNumber(), tok); break;
            case LIT_CHAR: fprintf(stderr,"%d - LIT_CHAR: %d \n", getLineNumber(), tok); break;
   			case LIT_STRING: fprintf(stderr,"%d - LIT_STRING: %d \n", getLineNumber(), tok); break;
   			case TOKEN_ERROR: fprintf(stderr,"%d - TOKEN_ERROR: %d \n", getLineNumber(), tok); break;
			default: fprintf(stderr, "%d - Char Especial: %d \n", getLineNumber(), tok); break;
		}
	}
	//só roda o código se o arquivo de hashtable for incluso
	print_table(g_table);

	return 0;
}
