%{
/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/
	//have to use abs path

	#include <stdio.h>
	#include <stdlib.h>
	#include "../../src/data_structures/hashtable.h"
	#include "../../src/data_structures/ast.h"
	extern int getLineNumber(void);


	//creio que o erro informado só ocorre na versão de MAC OS do yacc, pois em linux n há warnings
	//https://lists.gnu.org/r/bug-bison/2022-01/msg00006.html
	int yylex(void);
	int yyerror(char* err);

	AstNode* g_syntax_tree;
	int lexical_errors = 0;

	#define ERR(...) fprintf(stderr, "  %d: ", getLineNumber()); fprintf(stderr, __VA_ARGS__); lexical_errors++
%}

%union{
	HashNode *symbol;
	AstNode *ast;
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

%start inicio

%type <symbol> literal
%type <ast> inicio
%type <ast> def
%type <ast> fun
%type <ast> tipo
%type <ast> declaracao
%type <ast> valorLista
%type <ast> paramLista
%type <ast> paramListaContinua
%type <ast> exp
%type <ast> callFunParam
%type <ast> callFunParamContinua
%type <ast> code
%type <ast> startCmd
%type <ast> cmd
%type <ast> flow
%type <ast> action


%%
inicio: def		{ g_syntax_tree = TNODE(AST_INICIO, $1); } 
	| def fun   { g_syntax_tree = TNODE(AST_INICIO_AND, $1, $2); }
	|           { g_syntax_tree = NULL; }
	;

def: declaracao      { $$ = TNODE(AST_DEF, $1); }
	| declaracao def { $$ = TNODE(AST_DEF_AND, $1, $2); }
	| error def 	 { $$ = NULL; ERR("erro na escrita do corpo da função.\n"); }
	;

fun: code 		{ $$ = TNODE(AST_FUN, $1); }
	| code fun	{ $$ = TNODE(AST_FUN_AND, $1, $2); }
	;

tipo: KW_INT 	{ $$ = TTEND(AST_KW_INT); }
	| KW_CHAR   { $$ = TTEND(AST_KW_CHAR); }
	| KW_FLOAT 	{ $$ = TTEND(AST_KW_FLOAT); }
	;

literal: LIT_INT { $$ = $1; }
	| LIT_CHAR 	 { $$ = $1; }
	| LIT_REAL   { $$ = $1; }
	;

declaracao: tipo TK_IDENTIFIER '=' literal ';'          { $$ = TNODE(AST_DECLARACAO, $1, $2, $4); }
	| tipo TK_IDENTIFIER '[' LIT_INT ']' valorLista ';' { $$ = TNODE(AST_DECLARACAO_ARRAY, $1, $2, $4, $6); }
	| tipo TK_IDENTIFIER '(' paramLista ')' ';'         { $$ = TNODE(AST_DECLARACAO_FUN, $1, $2, $4); }
	
	| tipo TK_IDENTIFIER '(' error ')' ';'         		{ $$ = NULL; ERR("param decl\n"); }
	| tipo TK_IDENTIFIER '[' LIT_INT ']' error ';' 		{ $$ = NULL; ERR("vec decl\n"); }
	| tipo TK_IDENTIFIER error ';' 						{ $$ = NULL; ERR("Erro na declaração. Faltando assinalação a um literal.\n"); }
	| tipo TK_IDENTIFIER '=' literal error				{ $$ = NULL; ERR("Erro na declaração. Faltando ; na linha anterior.\n"); }
	;

valorLista: literal valorLista { $$ = TNODE(AST_VALOR_LISTA, $1, $2); }
	| error valorLista 		   { $$ = NULL; ERR("Erro na declaração da lista de literais.\n"); }
	|						   { $$ = NULL; }
	;

paramLista: tipo TK_IDENTIFIER paramListaContinua { $$ = TNODE(AST_PARAM_LISTA, $1, $2, $3); }
	| error paramListaContinua 				      { $$ = NULL; ERR("Erro na declaração dos parâmetros da função.\n"); }
	|						   					  { $$ = NULL; }
	;

paramListaContinua: ',' tipo TK_IDENTIFIER paramListaContinua { $$ = TNODE(AST_PARAM_LISTA_CONTINUA, $2, $3, $4); }
	| ',' error paramListaContinua 				    		  { $$ = NULL; ERR("Erro na declaração dos parâmetros da função.\n"); }
	|														  { $$ = NULL; }
	;

exp : TK_IDENTIFIER                      { $$ = TNODE(AST_EXP_IDENTIFIER, $1); } 
    | literal                            { $$ = TNODE(AST_EXP_LITERAL, $1); }
    | TK_IDENTIFIER '[' exp ']'          { $$ = TNODE(AST_EXP_ARRAY_ACESS, $1, $3); }
    | TK_IDENTIFIER '(' callFunParam ')' { $$ = TNODE(AST_EXP_CALL_FUN, $1, $3); }
    | KW_INPUT '(' tipo ')'              { $$ = TNODE(AST_EXP_INPUT, $3); }
	| '(' exp ')'                        { $$ = $2; }
    | '~' exp                            { $$ = TNODE(AST_EXP_NEG, $2); }
    | exp '+' exp                        { $$ = TNODE(AST_EXP_SUM, $1, $3); }
    | exp '-' exp                        { $$ = TNODE(AST_EXP_SUB, $1, $3); }
    | exp '*' exp                        { $$ = TNODE(AST_EXP_MUL, $1, $3); }
    | exp '/' exp                        { $$ = TNODE(AST_EXP_DIV, $1, $3); }
    | exp '<' exp                        { $$ = TNODE(AST_EXP_LESS, $1, $3); }
    | exp '>' exp                        { $$ = TNODE(AST_EXP_GREAT, $1, $3); }
    | exp OPERATOR_LE exp                { $$ = TNODE(AST_EXP_LE, $1, $3); }
    | exp OPERATOR_GE exp                { $$ = TNODE(AST_EXP_GE, $1, $3); }
    | exp OPERATOR_EQ exp                { $$ = TNODE(AST_EXP_EQ, $1, $3); }
    | exp OPERATOR_DIF exp               { $$ = TNODE(AST_EXP_DIF, $1, $3); }
    | exp '&' exp                        { $$ = TNODE(AST_EXP_AND, $1, $3); }
    | exp '|' exp                        { $$ = TNODE(AST_EXP_OR, $1, $3); }
    ;

callFunParam: exp callFunParamContinua  { $$ = TNODE(AST_CALL_FUN_PARAM, $1, $2); }
	| error callFunParamContinua        { $$ = NULL; ERR("Erro no parâmetro da chamada da função.\n"); }
	|                       			{ $$ = NULL; }
	;

callFunParamContinua: ',' exp callFunParamContinua { $$ = TNODE(AST_CALL_FUN_PARAM_CONTINUA, $2, $3); }
	| ',' error callFunParamContinua			   { $$ = NULL; ERR("Erro no parâmetro da chamada da função.\n");}
	|						                       { $$ = NULL; }
	;

code: KW_CODE TK_IDENTIFIER startCmd  { $$ = TNODE(AST_CODE, $2, $3); }
	| TK_IDENTIFIER error startCmd 	  { $$ = NULL; ERR("erro na declaração de função.\n"); }
	;

startCmd: '{' cmd '}'  { $$ = TNODE(AST_START_CMD_BLOCK, $2); }
    | action           { $$ = TNODE(AST_START_CMD_ACTION, $1); }
    | flow             { $$ = TNODE(AST_START_CMD_FLOW, $1); }
	;

cmd: '{' cmd '}' cmd   { $$ = TNODE(AST_CMD_BLOCK, $2, $4); }
	| action cmd       { $$ = TNODE(AST_CMD_ACTION, $1, $2); }
	| flow cmd         { $$ = TNODE(AST_CMD_FLOW, $1, $2); }
	|                  { $$ = NULL; }
	; 

flow: KW_WHILE '(' exp ')' startCmd                 { $$ = TNODE(AST_FLOW_WHILE, $3, $5); }
	| KW_IF '(' exp ')' startCmd                    { $$ = TNODE(AST_FLOW_IF, $3, $5); }
	| KW_IF '(' exp ')' startCmd KW_ELSE startCmd   { $$ = TNODE(AST_FLOW_ELSEIF, $3, $5, $7); }
	| KW_WHILE  error startCmd                      { $$ = NULL; ERR("Erro na expressão while.\n"); }
	| KW_IF error startCmd                          { $$ = NULL; ERR("Erro na expressão if.\n"); }
	| KW_IF error startCmd KW_ELSE startCmd         { $$ = NULL; ERR("Erro na expressão elseif.\n"); }
	;

action: TK_IDENTIFIER '=' exp ';'            { $$ = TNODE(AST_ACTION_ASSIGN, $1, $3); } 
    | TK_IDENTIFIER '[' exp ']' '=' exp ';'  { $$ = TNODE(AST_ACTION_ASSIGN_ARRAY, $1, $3, $6); } 
    | KW_RETURN exp ';'                      { $$ = TNODE(AST_ACTION_RETURN, $2); }
    | KW_PRINT exp ';'                       { $$ = TNODE(AST_ACTION_PRINT_EXP, $2); }
    | KW_PRINT LIT_STRING ';'                { $$ = TNODE(AST_ACTION_PRINT_STRING, $2); }
    | ';'                                    { $$ = TTEND(AST_SEMICOLON); }            
	| TK_IDENTIFIER '=' error ';'            { $$ = NULL; ERR("Erro na expressão de assinalamento.\n"); }
	| TK_IDENTIFIER '[' error ';'            { $$ = NULL; ERR("Erro na expressão de assinalamento de vetor.\n"); }
    | KW_RETURN error ';'                    { $$ = NULL; ERR("Erro na expressão return.\n"); }
    | KW_PRINT error ';'                     { $$ = NULL; ERR("Erro na expressão print.\n"); }
	;

%%

int yyerror(char* err){
	fprintf(stderr, "linha %d: %s\n", getLineNumber(), err);
	return 0;
}
