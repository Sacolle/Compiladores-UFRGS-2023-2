#include "gen_asm.h"

#include <stdlib.h>
#include <string.h>

//TODO: make imediate acess, fix undeclared funcion vars
#define PANICIF(c, info) if(c) { fprintf(stderr, info); exit(7);}


#define NEWLINE fprintf(file, "\n")

#include "../data_structures/hashtable.h"
#include "../data_structures/btree.h"
#include "semantics.h"

#define WRITE(...) fprintf(file, __VA_ARGS__)
#define INST(a, b, c) fprintf(file, "%s %s, %s\n", a, b, c)
#define INST2(a, b) fprintf(file, "%s %s\n", a, b)
#define VAR(node) var(alias_tree, node)

//base https://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html

extern HashTable* g_table;
extern AstNode* g_syntax_tree;

char buff[1024];
char* var(Btree* tree, HashNode* node){
	char* alias = NULL;
	switch(node->val){
		case SYMBOL_LIT_INTE: sprintf(buff, "$%s", node->key); break;
		case SYMBOL_LIT_REAL: sprintf(buff, "$%d", (int) atof(node->key)); break;
		case SYMBOL_LIT_CARA: sprintf(buff, "$%d", node->key[1]); break;
		default:
			alias = btree_get(tree, node->key); 
			if(alias == NULL)
				sprintf(buff, "%s(,1)", node->key); 
			else
				sprintf(buff, "%s", alias); 
			break;
	}
	return buff;
}


void insert_identifiers(FILE* file, AstNode* def, Btree* alias_tree);

void make_header(FILE* file, Btree* alias_tree){
	WRITE(".data\n");
	//faz os literais
	HashNode* node;
	for(int i = 0; i < g_table->size; i++){
		node = g_table->elems[i]; 
		while(node != NULL){
			switch(node->val){
			case SYMBOL_LIT_STRING:
				WRITE("str_%p:\n\t.asciz %s\n", ((void*) node->key), node->key);
				break;
			case SYMBOL_TEMP_IDENTIFIER:
				if(btree_get(alias_tree, node->key) == NULL){
					WRITE("%s:\n\t.long 0\n", node->key);
				}
				break;
			default: break;
			}
			node = node->next;
		}
	}
	if(g_syntax_tree == NULL) return;
	
	AstNode* declarations = (AstNode*) g_syntax_tree->children[0];
	while(declarations != NULL){
		insert_identifiers(file, (AstNode*) declarations->children[0], alias_tree);
		declarations = (AstNode*) declarations->children[1];
	};
}

void literal_print(FILE* file, HashNode* lit){
	switch(lit->val){
		case SYMBOL_LIT_INTE: WRITE("%s", lit->key); break;
		case SYMBOL_LIT_REAL: WRITE("%d", (int) atof(lit->key)); break;
		case SYMBOL_LIT_CARA: WRITE("%d", lit->key[1]); break;
		default: WRITE("%s", lit->key); break;
	}
}

int make_fun_arg_alias(Btree* alias_tree, AstNode* list){
	int res = 0;
	if(list == NULL) return res;
	res = 1 + make_fun_arg_alias(alias_tree, (AstNode*) list->children[2]);
	
	char* buff = (char*) malloc(sizeof(char) * 10);
	sprintf(buff, "+%d(%%ebp)", 4 + 4 * res);
	btree_insert(alias_tree, list->children[1]->leaf.key, buff);
	return res;
}	

void insert_identifiers(FILE* file, AstNode* def, Btree* alias_tree){
	switch(def->type){
		case AST_DECLARACAO: 
				WRITE("%s:\n\t.long ", def->children[1]->leaf.key);
				literal_print(file, (HashNode*) def->children[2]);
				NEWLINE;
			break;
		case AST_DECLARACAO_ARRAY:
			WRITE("%s:\n\t", def->children[1]->leaf.key);
			if(!def->children[3]){
				WRITE(".zero %d\n", atoi(def->children[2]->leaf.key) * 4);
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
			make_fun_arg_alias(alias_tree, def);
			break;
		default: break;
	}
}

void write_math_binop_asm(FILE* file, Btree* alias_tree, Tac* op){
	//TODO: test if immediate here
	//WRITE("mov _%s(,1), %%eax\n", op->op1->key);
	//WRITE("mov _%s(,1), %%ebx\n", op->op2->key);
	INST("mov", VAR(op->op1), "%eax");
	INST("mov", VAR(op->op2), "%ebx");
	
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
		// if CF = 1 xor if ZF = 1, save number 1 in eax else eax 0 
		/*
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x0100, %%eax\n");
		WRITE("shr $8, %%eax\n");
		*/
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
		WRITE("xor $1, %%ebx\n"); //se ZF = 1, set ZF = 0
		//if any is 1
		WRITE("and %%ebx, %%eax\n");

		break;
	case TAC_GREAT: 
		// if CF = 0 xor if ZF = 1, save 1 in eax 
		/*
		WRITE("cmp %%ebx, %%eax\n");
		WRITE("mov $0, %%eax\n");
		WRITE("lahf\n");
		WRITE("and $0x0100, %%eax\n");
		WRITE("xor $0x0100, %%eax\n"); //se CF = 0, ent retorna 0x0100
		WRITE("shr $8, %%eax\n");
		*/
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
		WRITE("xor $1, %%ebx\n"); //se ZF = 1, set ZF = 0
		//if any is 1
		WRITE("and %%ebx, %%eax\n");
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

	INST("mov", "%eax", VAR(op->out));
}
int num_of_args(AstNode *list){
	if(list == NULL) return 0;
	return 1 + num_of_args((AstNode*) list->children[2]);
}
//Analisa todos os tacs dentro de uma função e checa o tempo de vida das variaveis __temp
//com isso, realiza um alias para elas serem usadas no stack baseado no tempo de vida da variável
Tac* parse_function(Tac* tac, Btree* alias_tree){
	if(tac->type != TAC_BEGINFUN) return tac;
	HashNode* function_node = tac->out;

	LtMap* map = ltmap_create();
	char** ocurrences = (char**) calloc(128, sizeof(char*));

	int instant = 0;
	LtMapNode* node;
	while(tac->type != TAC_ENDFUN){
		if(tac->op2 != NULL && tac->op2->val == SYMBOL_TEMP_IDENTIFIER){
			if((node = ltmap_get(map, tac->op2->key)) == NULL){
				//se não tem o nodo, insere
				ltmap_insert(map, tac->op2->key, instant, instant);
				ocurrences[instant] = tac->op2->key;
			}else{
				//se tem, atualiza
				node->last_use = instant;
			}
		}
		if(tac->op1 != NULL && tac->op1->val == SYMBOL_TEMP_IDENTIFIER){
			if((node = ltmap_get(map, tac->op1->key)) == NULL){
				//se não tem o nodo, insere
				ltmap_insert(map, tac->op1->key, instant, instant);
				ocurrences[instant] = tac->op1->key;
			}else{
				//se tem, atualiza
				node->last_use = instant;
			}
		}
		if(tac->out != NULL && tac->out->val == SYMBOL_TEMP_IDENTIFIER){
			if((node = ltmap_get(map, tac->out->key)) == NULL){
				//se não tem o nodo, insere
				ltmap_insert(map, tac->out->key, instant, instant);
				ocurrences[instant] = tac->out->key;
			}else{
				//se tem, atualiza
				node->last_use = instant;
			}
		}
		//extra anotation for inlining latter
		//checks if a function calls other functions
		//or if it has any jumps
		if(tac->type == TAC_CALL || tac->type == TAC_LABEL){
			function_node->inlinable = 0;
		}

		instant++;
		PANICIF(instant > 127, "size of ocurrences buffer exceded\n");
		tac = tac->next;
	}	
	#define SLOTSIZE 12
	char slots[SLOTSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int added = 0;
	int num_of_local_var = 0;
	for(int i = 0; i < instant; i++){
		for(int j = 0; j < SLOTSIZE; j++){
			if(ocurrences[i] != NULL && slots[j] == 0){
				if(!added){
					char* buff = (char*) malloc(sizeof(char) * 10);
					sprintf(buff, "-%d(%%ebp)", 4 + 4 * j);
					btree_insert(alias_tree, ocurrences[i], buff);
					num_of_local_var = j > num_of_local_var ? j : num_of_local_var;

					slots[j] = ltmap_get(map, ocurrences[i])->last_use -
						ltmap_get(map, ocurrences[i])->first_use + 1;
					added = 1;
				}
			}
			slots[j] = slots[j] == 0 ? 0 : slots[j] - 1;
		}
		PANICIF(ocurrences[i] != NULL && !added, "Buffer de slot é muito pequeno\n");
		//fprintf(stderr, "%d: ", i);
		//for(int j = 0; j < SLOTSIZE; j++)fprintf(stderr, "%d,", slots[j]);
		//fprintf(stderr, "\n");
		added = 0;
	}
	//fprintf(stderr, "o número de variáveis locais é: %d\n\n", num_of_local_var + 1);
	//a var num_of_local_var é o índice máximo do vetor de slots, ou seja, deve-se somar + 1 
	function_node->num_of_local_var = num_of_local_var + 1;

	//ltmap_print(map);
	ltmap_free(map);
	free(ocurrences);
	return tac;
}

void parse_functions(Tac* tac_list, Btree* alias_tree){
	Tac* tac = tac_list;
	while((tac = parse_function(tac, alias_tree)) != NULL){
		tac = tac->next;
		if(tac == NULL) return;
	}
}

int operand_instruction(int type){
	switch (type){
	case TAC_MOVE:
	case TAC_ADD:
	case TAC_SUB:
	case TAC_MUL:
	case TAC_DIV:
	case TAC_CALL :
	case TAC_ARG :
	case TAC_READ :
		return 1;
	default:
		return 0;
	}
}


void tac_mov_elimination(Tac* tac_list){
	Tac* tac = tac_list;
	Tac* to_free_tac = NULL;
	while(tac != NULL){
		if(tac->type == TAC_MOVE && 
		operand_instruction(tac->prev->type) && 
		tac->op1 == tac->prev->out)
		{
			tac->prev->out = tac->out;

			tac->prev->next = tac->next;
			tac->next->prev = tac->prev;
			to_free_tac = tac;
			tac = tac->next;
			free(to_free_tac);
		}else{
			tac = tac->next;
		}
	}
}
/*
Tac* tac_find_funtion(Tac* tac_list, HashNode* name){
	for(Tac* tac = tac_list; tac; tac = tac->next){
		if(tac->type == TAC_BEGINFUN && tac->out == name) return tac;
	}
}
HashNode* get_nth_arg(int n, AstNode* list){
	if(n == 0) return (HashNode*) list->children[2];
	return get_nth_arg(n - 1, (HashNode*) list->children[2]);
}
/
void tac_inline_functions(Tac* tac_list){
	Tac* tac = tac_list;
	Tac* find_arg = tac_list;
	while(tac != NULL){
		if(tac->type == TAC_CALL && tac->op1->inlinable){
			//get the num args before
			//change them to mov to the arg
			for(int n = num_of_args((AstNode*) tac->op1->param_list); n > 0; n--){
				find_arg = tac->prev;	
				while(find_arg->type != TAC_ARG) find_arg->prev;
				//change to mov
				find_arg->type = TAC_MOVE;
				find_arg->op1 = find_arg->out;
				find_arg->out = get_nth_arg(n - 1, (AstNode*) tac->op1->param_list);
			}
			//change call to the body of the function
			//the return becomes, mov __temp(from teh tac_call), __temp(from the ret)
		}
		tac = tac->next;
	}
}*/

void generate_asm(FILE* file, Tac* tac_list){
	//print_tac(tac_list);
	//fprintf(stderr, "\nEliminando MOVS desnecessários.\n\n");
	tac_mov_elimination(tac_list);
	print_tac(tac_list);

	Btree* alias_tree = btree_create("mmmmmm", "default");

	parse_functions(tac_list, alias_tree);

	make_header(file, alias_tree);	

	WRITE("__fmt_int__:\n\t.string \"%%d\"\n");
	WRITE("__scan_val__:\n\t.long 0\n");

	WRITE("\n.text\n");

	//btree_print(alias_tree);

	int has_return = 0;
	for(Tac* tac = tac_list; tac; tac = tac->next){
		switch (tac->type){
		case TAC_MOVE:
			//TODO: could be immediate
			//WRITE("mov %s(,1), %%eax\n", tac->op1->key);
			INST("mov", VAR(tac->op1), "%eax");
			INST("mov", "%eax", VAR(tac->out));
			break;
		case TAC_MOVE_VEC: //coloca o valor para dentro do vetor
			INST("mov", VAR(tac->op2), "%eax");
			INST("mov", VAR(tac->op1), "%eax");
			//args cannot be vec, so no need to check for alias
			WRITE("mov %%eax, %s(, %%ebx, 4)\n", tac->out->key);
			break;
		case TAC_ACESS_VEC:
			INST("mov", VAR(tac->op2), "%ebx");
			WRITE("mov %s(, %%ebx, 4), %%eax\n", tac->op1->key);
			INST("mov", "%eax", VAR(tac->out));
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
			write_math_binop_asm(file, alias_tree, tac);
			break;
		case TAC_NEG:
			INST("mov", VAR(tac->op1), "%eax");
			WRITE("xor $1, %%eax\n");
			INST("mov", "%eax", VAR(tac->out));
			break;
		case TAC_LABEL: WRITE("%s:\n", tac->out->key); break;
		case TAC_IFZ:
			INST("mov", VAR(tac->op1), "%eax");
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
			WRITE("sub $%d, %%esp\n", tac->out->num_of_local_var * 4);
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
			if(tac->type == TAC_RET){
				//TODO: could be immediate
				//WRITE("mov %s(,1), %%eax\n", tac->op1->key);
				INST("mov", VAR(tac->out), "%eax");
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
		*/	
			WRITE("call %s\n", tac->op1->key);
			WRITE("add $%d, %%esp\n", 4 * num_of_args(tac->op1->param_list));
			INST("mov", "%eax", VAR(tac->out));
			break;
		case TAC_ARG: //empilha para depois desempilhar antes de chamar a função
			INST2("push", VAR(tac->out));
			break;
		case TAC_PRINT: //printf * ou valor
			if(tac->out->val == SYMBOL_LIT_STRING){
				WRITE("push $str_%p\n", (void*) tac->out->key);
				WRITE("call printf\n");
				WRITE("add $4, %%esp\n");
			}else{
				//TODO: could be immediate
				INST2("push", VAR(tac->out));

				WRITE("push $__fmt_int__\n");
				WRITE("call printf\n");
				WRITE("add $8, %%esp\n");
			}
			break;
		case TAC_READ: //scanf
			//WRITE("push $%s\n", tac->out->key); 
			WRITE("push $__scan_val__\n");
			WRITE("push $__fmt_int__\n");
			WRITE("call scanf\n");
			WRITE("add $8, %%esp\n");
			WRITE("mov __scan_val__(,1), %%eax\n");
			INST("mov", "%eax", VAR(tac->out));
			break;
		default:
			break;
		}
	}
}
