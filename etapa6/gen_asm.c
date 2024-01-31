#include "gen_asm.h"

#define WRITE(...) fprintf(file, __VA_ARGS__)

#include "hashtable.h"
#include "semantics.h"


//base https://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html

extern HashTable* g_table;

void make_header(FILE* file){
	HashNode* node;
	for(int i = 0; i < g_table->size; i++){
		node = g_table->elems[i]; 
		while(node != NULL){
			switch(node->val){
			case SYMBOL_LIT_REAL:
			case SYMBOL_LIT_INTE:
				break;
			case SYMBOL_LIT_CARA:
				break;
			case SYMBOL_LIT_STRING:
				break;
			case SYMBOL_IDENTIFIER:
				break;
			}
			node = node->next;
		}
	}
}

void generate_asm(FILE* file, Tac* tac_list){
	make_header(file);	



}
