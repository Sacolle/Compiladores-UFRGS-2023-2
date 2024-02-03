#include "gen_asm.h"

#include <stdlib.h>
#include <string.h>

//TODO: make imediate acess, fix undeclared funcion vars

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
				WRITE("xyz%p:\n\t.asciz %s\n", ((void*) node->key), node->key);
				break;
			case SYMBOL_TEMP_IDENTIFIER: 
				WRITE("_%s:\n\t.long 0\n", node->key);
				break;
			default: break;
			}
			node = node->next;
		}
	}
	if(g_syntax_tree == NULL) return;
	
	AstNode* declarations = (AstNode*) g_syntax_tree->children[0];
	insert_identifiers(file, (AstNode*) declarations->children[0]);
	declarations = (AstNode*) declarations->children[1];
	if(declarations == NULL) return;
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
				WRITE("_%s:\n\t.long ", def->children[1]->leaf.key);
				literal_print(file, (HashNode*) def->children[2]);
				NEWLINE;
			break;
		case AST_DECLARACAO_ARRAY:
			WRITE("_%s:\n\t", def->children[1]->leaf.key);
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
		case AST_DECLARACAO_FUN:
			def = (AstNode*) def->children[2];
			while(def){
				WRITE("_%s:\n\t.long 0\n", def->children[1]->leaf.key);
				def = (AstNode*) def->children[2];
			}
			break;
		default: break;
	}
}
//gcc -m32 arquivo

int g_cmp_label = 0;

void write_math_binop_asm(FILE* file, Tac* op){
	WRITE("mov _%s(,1), %%eax\n", op->op1->key);
	WRITE("mov _%s(,1), %%ebx\n", op->op2->key);
	switch (op->type){
	case TAC_ADD: WRITE("add %%ebx, %%eax\n"); break;
	case TAC_SUB: WRITE("sub %%ebx, %%eax\n"); break;
	case TAC_MUL: WRITE("imul %%ebx, %%eax\n"); break;
	case TAC_DIV:
		WRITE("mov $0, %%edx\n");
		//idiv %reg divide o valor de EDX:EAX por %reg, devolvendo o quociente em %eax
		WRITE("idiv %%ebx\n");
		break;
	// https://stackoverflow.com/questions/1406783/how-to-read-and-write-x86-flags-registers-directly
	// https://en.wikipedia.org/wiki/FLAGS_register
	case TAC_LESS: 
		// if CF = 1, save number 1 in eax else eax 0 
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x0100, %%eax\n");
		WRITE("shr $8, %%eax\n");
		break;
	case TAC_GREAT: 
		// if CF = 0, save 1 in eax 
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x0100, %%eax\n");
		WRITE("xor $0x0100, %%eax\n"); //se CF = 0, ent retorna 0x0100
		WRITE("shr $8, %%eax\n");
		break;
	case TAC_LE: 
		// if CF = 1, save 1 in eax, or if ZF = 1
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("mov %%eax, %%ebx\n");
		//check carry flag
		WRITE("and $0x0100, %%eax\n");
		WRITE("shr $8, %%eax\n");
		//check zero flag
		WRITE("and $0x4000, %%ebx\n");
		WRITE("shr $14, %%ebx\n");
		//if any is 1
		WRITE("or %%ebx, %%eax\n");
		break;
	case TAC_GE: 
		// if CF = 0, save 1 in eax and set ZF = 0
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("mov %%eax, %%ebx\n");
		//check carry flag
		WRITE("and $0x0100, %%eax\n");
		WRITE("xor $0x0100, %%eax\n");
		WRITE("shr $8, %%eax\n");
		//check zero flag
		WRITE("and $0x4000, %%ebx\n");
		WRITE("shr $14, %%ebx\n");
		//if any is 1
		WRITE("or %%ebx, %%eax\n");
		break;
	case TAC_EQ: 
		//cmp ZF = 1 CF = 0
		WRITE("cmp %%ebx, %%eax\n"); 
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x4000, %%eax\n");
		WRITE("shr $14, %%eax\n");
		break;
	case TAC_DIF: 
		//not ZF
		WRITE("cmp %%ebx, %%eax\n"); 
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x4000, %%eax\n");
		WRITE("xor $0x4000, %%eax\n");
		WRITE("shr $14, %%eax\n");
		break;
	case TAC_AND: WRITE("and %%ebx, %%eax\n"); break;
	case TAC_OR: WRITE("or %%ebx, %%eax\n"); break;
	}

	WRITE("mov %%eax, _%s(,1)\n", op->out->key);
}


int assing_to_func_args(FILE* file, AstNode* list){
	int res = 0;
	if(list == NULL) return res;
	res = 1 + assing_to_func_args(file, (AstNode*) list->children[2]);
	WRITE("pop %%ecx\n");
	WRITE("mov %%ecx, _%s(,1)\n", list->children[1]->leaf.key);
	return res;
}	

void generate_asm(FILE* file, Tac* tac_list){
	make_header(file);	
	WRITE("__scan_int__:\n\t.string \"%%d\"\n");

	WRITE("\n.text\n");

	int has_return = 0;
	for(Tac* tac = tac_list; tac; tac = tac->next){
		switch (tac->type){
		case TAC_MOVE:
			WRITE("mov _%s(,1), %%eax\n", tac->op1->key);
			WRITE("mov %%eax, _%s(,1)\n", tac->out->key);
			break;
		case TAC_MOVE_VEC: //coloca o valor para dentro do vetor
			WRITE("mov _%s(,1), %%eax\n", tac->op2->key);
			WRITE("mov _%s(,1), %%ebx\n", tac->op1->key);
			WRITE("mov %%eax, _%s(, %%ebx, 4)\n", tac->out->key); //NOTE: check if 4 or 1
			break;
		case TAC_ACESS_VEC:
			WRITE("mov _%s(,1), %%ebx\n", tac->op2->key);
			WRITE("mov _%s(, %%ebx, 4), %%eax\n", tac->op1->key);
			WRITE("mov %%eax, _%s(,1)\n", tac->out->key);
			break;
		case TAC_ADD:
		case TAC_SUB:
		case TAC_MUL:
		case TAC_DIV:
		case TAC_LESS:
		case TAC_GREAT:
		case TAC_LE:
		case TAC_GE:
		case TAC_EQ:
		case TAC_DIF:
		case TAC_AND:
		case TAC_OR:
			write_math_binop_asm(file, tac);
			break;
		case TAC_NEG:
			WRITE("mov _%s(,1), %%eax\n", tac->op1->key);
			WRITE("xor $1, %%eax\n");
			WRITE("mov %%eax, _%s(,1)\n", tac->out->key);
			break;
		case TAC_LABEL: WRITE("%s:\n", tac->out->key); break;
		case TAC_IFZ:
			WRITE("mov _%s(,1), %%eax\n", tac->op1->key);
			WRITE("cmp $0, %%eax\n");
			WRITE("jz %s\n", tac->out->key);
			break;
		case TAC_JUMP:
			WRITE("jmp %s\n", tac->out->key);
			break;
		case TAC_BEGINFUN: //if main, dont decorate
			WRITE("\n.globl %s\n", tac->out->key);
			if(strcmp("main", tac->out->key) != 0){
				WRITE(".type %s, @function\n", tac->out->key);
			}
			WRITE("%s:\n", tac->out->key);
		/* callee convention
			empurra o base pointer para o stack e copia o esp para o ebp 
			push %ebp
			mov %esp, %ebp
			para alocar valores no stack (variaveis locais), subtraí-se do %esp os bytes usados
			empurra os registradores do chamador no stack, %ebx, %edi, %esi 
		*/
			WRITE("push %%ebp\n");
			WRITE("mov %%esp, %%ebp\n");
			//NOTE: não estou salvando nenhum reg
			break;
		case TAC_ENDFUN: //NOTE: as funções devem ter return explicito
			if(has_return){
				has_return = 0;
				break;
			}
		case TAC_RET: //calling convention
		/* return convention
			deixa o valor de retorno em %eax,
			restaura os valores de %edi, %esi (pop the stack em ordem inversa)
			dealoca as variáveis locais, mov %ebp, %esp
			pop %ebp do stack
		*/	
			has_return = 1;
			if(tac->op1 != NULL){
				WRITE("mov _%s(,1), %%eax\n", tac->op1->key);
			}else{
				//caso seja um ENDFUN
				WRITE("mov $0, %%eax\n");
			}
			//retorno dos registradores salvos do chamador
			//WRITE("pop %%esi\n");
			//WRITE("pop %%edi\n");
			WRITE("mov %%ebp, %%esp\n");
			WRITE("pop %%ebp\n");
			WRITE("ret\n");
			break;
		case TAC_CALL: //calling convention
		/* caller convention
			save registers eax, ecx, edx
			push the params (ordem inversa)
			depois da call deve-se somar ao esp 4 * o número de parâmetros usados
		*/	WRITE("\n/* pops */\n");
			assing_to_func_args(file, tac->op1->param_list);
			WRITE("/* pops */\n");

			WRITE("call %s\n", tac->op1->key);
			WRITE("mov %%eax, _%s(,1)\n", tac->out->key);
			break;
		case TAC_ARG: //stack based
		/*
			os valores estarão disponíveis na função como %ebp+4, +8, etc
		*/
			WRITE("push _%s(,1)\n", tac->out->key);
			break;
		case TAC_PRINT: //printf * ou valor
			if(tac->out->val == SYMBOL_LIT_STRING){
				WRITE("push $xyz%p\n", (void*) tac->out->key);
				WRITE("call printf\n");
				WRITE("add $4, %%esp\n");
			}else{
				WRITE("push _%s(,1)\n", tac->out->key);
				WRITE("push $__scan_int__\n");
				WRITE("call printf\n");
				WRITE("add $8, %%esp\n");
			}
			break;
		case TAC_READ: //scanf
			WRITE("push $_%s\n", tac->out->key); 
			WRITE("push $__scan_int__\n");
			WRITE("call scanf\n");
			WRITE("add $8, %%esp\n");
			break;
		default:
			break;
		}
	}
	/* main
	.globl main
	main:
	*/


	/* function call
	.globl area
	.type area, @function
	area:
	*/

}
