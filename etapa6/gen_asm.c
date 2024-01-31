#include "gen_asm.h"

#include <stdlib.h>

#define WRITE(...) fprintf(file, __VA_ARGS__)
#define NEWLINE fprintf(file, "\n")

#include "hashtable.h"
#include "semantics.h"



//base https://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html

extern HashTable* g_table;
extern AstNode* g_syntax_tree;

void insert_identifiers(FILE* file, AstNode* def);

void make_header(FILE* file){
	WRITE(".data\n");
	//faz os literais
	HashNode* node;
	for(int i = 0; i < g_table->size; i++){
		node = g_table->elems[i]; 
		while(node != NULL){
			switch(node->val){
			case SYMBOL_LIT_REAL:
			case SYMBOL_LIT_INTE:
				WRITE("_%s:\n\t.long %d\n", node->key, atoi(node->key));
				break;
			case SYMBOL_LIT_CARA:
				//ignora os casos com \n
				WRITE("_%c:\n\t.long %d\n", node->key[1], node->key[1]);
				break;
			case SYMBOL_LIT_STRING:
				WRITE("_%p:\n\t.string %s\n", ((void*) node->key), node->key);
				break;
			case SYMBOL_IDENTIFIER:
				break;
			}
			node = node->next;
		}
	}
	if(g_syntax_tree == NULL) return;
	
	AstNode* declarations = (AstNode*) g_syntax_tree->children[0];
	insert_identifiers(file, (AstNode*) declarations->children[0]);
	declarations = (AstNode*) declarations->children[1];
	while(declarations->children[1]){
		insert_identifiers(file, (AstNode*) declarations->children[0]);
		declarations = (AstNode*) declarations->children[1];
	}
}

void literal_print(FILE* file, HashNode* lit){
	switch(lit->val){
		case SYMBOL_LIT_INTE: WRITE("%s", lit->key); break;
		case SYMBOL_LIT_REAL: WRITE("%d", (int) atof(lit->key)); break;
		case SYMBOL_LIT_CARA: WRITE("%d", lit->key[1]); break;
		default: WRITE("%s", lit->key); break;
	}
}


void insert_identifiers(FILE* file, AstNode* def){
	switch(def->type){
		case AST_DECLARACAO: 
				WRITE("%s:\n\t.long ", def->children[1]->leaf.key);
				literal_print(file, (HashNode*) def->children[2]);
				NEWLINE;
			break;
		case AST_DECLARACAO_ARRAY:
			WRITE("%s:\n\t", def->children[1]->leaf.key);
			if(!def->children[3]){
				WRITE(".zero %s\n", def->children[2]->leaf.key);
			}else{
				def = (AstNode*) def->children[3]; //vai na lista de elementos
				WRITE(".long ");
				literal_print(file, (HashNode*) def->children[0]);
				while(def->children[1]){
					def = (AstNode*) def->children[1];
					WRITE(", ");
					literal_print(file, (HashNode*) def->children[0]);
				}
				NEWLINE;
			}
			break;
		default: break;
	}
}


void generate_asm(FILE* file, Tac* tac_list){
	make_header(file);	
}
