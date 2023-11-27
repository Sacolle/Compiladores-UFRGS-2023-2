%{
/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

	#include <stdio.h>
	#include <stdlib.h>
	#include "hashtable.h"
	extern int getLineNumber();


	//creio que o erro informado só ocorre na versão de MAC OS do yacc, pois em linux n há warnings
	//https://lists.gnu.org/r/bug-bison/2022-01/msg00006.html
	int yylex();
	int yyerror(char* err);
%}

%union{
	HashNode *symbol;
}


%token KW_CHAR
%token KW_INT       
%token KW_FLOAT     
%token KW_CODE      
%token KW_IF        
%token KW_ELSE      
%token KW_WHILE     
%token KW_INPUT     
%token KW_PRINT     
%token KW_RETURN    
%token OPERATOR_LE  
%token OPERATOR_GE  
%token OPERATOR_EQ  
%token OPERATOR_DIF 

%token <symbol> TK_IDENTIFIER
%token <symbol> LIT_INT      
%token <symbol> LIT_REAL     
%token <symbol> LIT_CHAR     
%token <symbol> LIT_STRING   

%token TOKEN_ERROR  

%left '|' '&' '~'
%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%left '+' '-'
%left '*' '/'

%%
inicio: def 
	| def fun 
	| 
	;

def: declaracao 
	| declaracao def
	;

fun: code 
	| code fun
	;

tipo: KW_INT | KW_CHAR | KW_FLOAT ;
literal: LIT_INT | LIT_CHAR | LIT_REAL ;

declaracao: tipo TK_IDENTIFIER '=' literal ';' 
	| tipo TK_IDENTIFIER '[' LIT_INT ']' valorLista ';'
	| tipo TK_IDENTIFIER '(' paramLista ')' ';'
	;

valorLista: literal valorLista 
	|
	;

paramLista: tipo TK_IDENTIFIER paramListaContinua
	|
	;

paramListaContinua: ',' tipo TK_IDENTIFIER paramListaContinua
	|
	;


exp : TK_IDENTIFIER
	| TK_IDENTIFIER '[' exp ']'
	| TK_IDENTIFIER '(' exp funParam ')'
	| KW_INPUT '(' tipo ')' 
	| LIT_INT
	| LIT_REAL
	| LIT_CHAR
	| '(' exp ')'
	| '~' exp
    | exp '+' exp
    | exp '-' exp
    | exp '*' exp
    | exp '/' exp
    | exp '<' exp
    | exp '>' exp
    | exp OPERATOR_LE exp
    | exp OPERATOR_GE exp
    | exp OPERATOR_EQ exp
    | exp OPERATOR_DIF exp
	| exp '&' exp
	| exp '|' exp
    ;

funParam: ',' exp funParam 
	|
	;

code: KW_CODE TK_IDENTIFIER startCmd;

startCmd: '{' cmd '}'
	| action
	| flow
	;

cmd: '{' cmd '}' cmd
	| action cmd
	| flow cmd
	| 
	; 

flow: KW_WHILE '(' exp ')' startCmd
	| KW_IF '(' exp ')' startCmd
	| KW_IF '(' exp ')' startCmd KW_ELSE startCmd
	;

action: TK_IDENTIFIER '=' exp ';' 
	| TK_IDENTIFIER '[' exp ']' '=' exp ';' 
	| KW_RETURN exp ';'
	| KW_PRINT exp ';'
	| KW_PRINT LIT_STRING ';'
	| ';'
	;

%%

int yyerror(char* err){
	fprintf(stderr, "Erro na linha %d\n\t%s\n", getLineNumber(), err);
	exit(3);	
}
