/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include <stdio.h>
#include "lex.yy.h"
#include "tokens.h"
#include "hashtable.h"
#include "semantics.h"
#include "ast.h"

extern FILE *yyin;
extern HashTable *g_table; //NOTE: g_table é declarada no scanner.l
extern AstNode *g_syntax_tree;
extern void initGlobalHashTable(void);
extern int yyparse();

int main(int argc, char** argv){
	if (argc < 2){
		fprintf(stderr, "ERRO: Arquivo de entrada não foi provido.\nChame: ./etapa4 input.txt\n");
		exit(1);
    }
  	if (0 == (yyin = fopen(argv[1], "r"))){
		fprintf(stderr, "ERRO: Não foi possível abrir o arquivo de entrada: %s\n",argv[1]);
		exit(2);
    }
	//inicializando a hashtable blobal
	initGlobalHashTable();
	yyparse();

	semantic_pass();

	//print_rebuild_file(output, g_syntax_tree);
	//print_ast(stderr, g_syntax_tree);
	//print_table(g_table);
	
	fprintf(stderr, "Sucesso na compilação!\n");
	exit(0);

	return 0;
}

