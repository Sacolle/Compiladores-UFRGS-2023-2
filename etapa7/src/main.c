/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include <stdio.h>

#include "../build/pre/lex.yy.h"
#include "./parsers/tokens.h"
#include "./parsers/semantics.h"
#include "./parsers/gen_asm.h"

#include "./data_structures/hashtable.h"
#include "./data_structures/ast.h"
#include "./data_structures/tac.h"

extern FILE *yyin;
extern HashTable *g_table; //NOTE: g_table é declarada no scanner.l
extern AstNode *g_syntax_tree;
extern void initGlobalHashTable(void);
extern int yyparse(void);
extern int g_error_counter;
extern int lexical_errors;

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
	if(lexical_errors){
		fprintf(stderr, "Finalizando o programa, %d erros semânticos encontrados.", lexical_errors);
		exit(5);
	}

	semantic_pass();

	
	Tac* tac = build_tac(g_syntax_tree);
	tac = reverse_tac(tac);
	//print_tac(tac);
	//print_table(g_table);

	//print_table(g_table);

	FILE* f = fopen("build/pre/out.s", "wb+");
	if(!f) { exit(5); }

	generate_asm(f, tac);

	//print_ast(stderr, g_syntax_tree);
	//print_rebuild_file(stdout, g_syntax_tree);

	if(g_error_counter){
		fprintf(stderr, "Falha na compilação! %d errors encontrados!\n", g_error_counter);
		exit(4);
	}else{
		fprintf(stderr, "Sucesso na compilação!\n");
		exit(0);
	}
	return 0;
}

