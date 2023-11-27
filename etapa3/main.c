/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include <stdio.h>
#include "lex.yy.h"
#include "tokens.h"
#include "hashtable.h"
#include "ast.h"

extern FILE *yyin;
extern HashTable *g_table; //NOTE: g_table é declarada no scanner.l
extern AstNode *g_syntax_tree;
extern void initMe(void);
extern int yyparse();

int main(int argc, char** argv){
	if (argc < 3){
		fprintf(stderr, "ERRO: Arquivo de entrada ou de saída não foi provido.\nChame: ./etapa3 input.txt output.txt\n");
		exit(1);
    }
  	if (0 == (yyin = fopen(argv[1], "r"))){
		fprintf(stderr, "ERRO: Não foi possível abrir o arquivo de entrada: %s\n",argv[1]);
		exit(1);
    }
	FILE* output;
	if((output = fopen(argv[2], "w")) == NULL){
		fprintf(stderr, "ERRO: Não foi possível abrir o arquivo de saída: %s\n",argv[2]);
		exit(1);
	}
	//inicializando a hashtable blobal
	initMe();
	yyparse();

	print_rebuild_file(output, g_syntax_tree);
	print_ast(stderr, g_syntax_tree);
	print_table(g_table);
	
	fclose(output);
	fprintf(stderr, "Sucesso na compilação!\n");
	exit(0);

	return 0;
}

