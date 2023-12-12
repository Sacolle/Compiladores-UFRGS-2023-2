
#include "semantics.h"

extern HashTable *g_table; 
extern AstNode *g_syntax_tree;

/*
* - todos os identificadores usados em assingmets devem ser declarados
* - identificadores só podem ser declarados 1 vez
* - há 3 tipos de identificadores: escalares, vetores, funções
* - há 3 tipos de dados para declarações: char, int e float
*     - booleanos não existem nas declarações. São gerados pelos operadores booleanos
*     - char e int são interoperáveis
*     - booleanos, inteiros (char e int) e floats são incompatíveis
* - há 4 tipos de literais: reais, inteiros, characteres e strings
* - o acesso de um vetor só pode ser com inteiros
*/

// declarações pass:
//	- salvar o tipo e a natureza de uma declaração
//		- salvar a lista de argumentos de uma função
//	- checar se o literal é valido para o tipo
//  	- checar se a inicialização do vetor está correta

//funcion validate
//	- Checar se a função é declarada
//	- checar os tipos de retorno

/*
    - ~~variáveis redeclaradas~~
    - ~~anotar tipo (natureza) nas tabela hash~~
    - ~~anotar tipo de dado (dataType) na tabela hash~~
    - ~~variáveis não-declaradas~~
    - ~~funções declaradas;~~
    - funções implementadas;
    - verificar natureza, se:
    	- escalares são usados como escalares
    	- vetores são usados como vetores
    	- funções são usadas como funções
    	- não esqueça de verificar no lado direito (expressões) e no lado esquerdo (atribuições)
    - ~~verificar tipos de dados nas expressões
    - ~~verificar argumentos de chamada de função versus parâmetros:
    	- ~~não pode haver menos argumentos~~
    	- ~~não pode haver mais argumentos~~
    	- ~~os tipos devem ser compatíveis (não iguais, lembre-se)~~
    - ~~verificar o tamanho e a lista de inicialização do vetor;~~
    - ~~verificar tipo do valor de retorno da função~~
    - ~~verificar índices dos vetores (não pode ser booleano, não pode ser real), tanto na
	expressão quanto na atribuição~~
*/
void parse_start_cmds(AstNode* start_cmd, HashNode* fn_identifier);
void parse_declaration(AstNode* ast);
void validate_function_args(AstNode* exp, AstNode* param_list);
int _parse_expression(AstNode* exp, HashNode* fn_identifier);
void parse_expression(AstNode* exp, int expected_type, HashNode* fn_identifier);
void parse_cmds(AstNode* cmd, HashNode* fn_identifier);
void parse_flow(AstNode* flow, HashNode* fn_identifier);
void parse_action(AstNode* action, HashNode* fn_identifier);
void parse_function(AstNode* ast);

char* type_to_string(int t){
	switch (t){
		case TYPE_BOOL: return "bool";
		case TYPE_INTE: return "int";
		case TYPE_REAL: return "real";
		default: exit(5); return "";
	}
}

int ast_type_to_type(int t){
	switch (t){
	case AST_KW_CHAR: 
	case AST_KW_INT: 
		return TYPE_INTE;
	case AST_KW_FLOAT: 
		return TYPE_REAL;
	default: printf("conversion failed, type received was %d >> 7 = %d", t, t >> 7); exit(5); return 0; //O switch acima é para ser exaustivo
	}
}

int hash_type_to_type(int t){
	switch (t){
	case SYMBOL_LIT_CARA: 
	case SYMBOL_LIT_INTE: 
		return TYPE_INTE;
	case SYMBOL_LIT_REAL: 
		return TYPE_REAL;
	default: return 0; 
	}
}

void parse_declaration(AstNode* ast){
	HashNode* identifier = &ast->children[1]->leaf;
	AstNode* list;
	int array_size, decl_type;
	//already has a type
	if(identifier->type){
		fprintf(stderr, "identificador %s já foi declarado neste programa.\n", identifier->key);
		exit(4);
	}
	decl_type = ast_type_to_type(ast->children[0]->branch.type);

	switch (ast->type){
	case AST_DECLARACAO: 
		identifier->type = NAT_VAL | decl_type;
		if(hash_type_to_type(ast->children[2]->leaf.val) != GET_TYPE(identifier)){
			fprintf(stderr, "identificador %s e literal %s tem tipos incompatíveis.\n",
				identifier->key, 
				ast->children[2]->leaf.key
			);
			exit(4);
		}
	break;
	case AST_DECLARACAO_ARRAY:
		identifier->type = NAT_VEC | decl_type;

		list = &ast->children[3]->branch;
		if(list == NULL) break; 
		
		array_size = atoi(ast->children[2]->leaf.key);

		for(int i = 0; i < array_size; i++){
			if(hash_type_to_type(list->children[0]->leaf.val) != GET_TYPE(identifier)){
				fprintf(stderr, "identificador %s e literal %s tem tipos incompatíveis.\n",
					identifier->key, 
					list->children[0]->leaf.key
				);
				exit(4);
			}
			// a lista tem menos filhos q declarado
			if((list->children[1] == NULL) && (i != array_size - 1)){
				fprintf(stderr, "Lista %s tem menos filhos do que declarado.\n", identifier->key);
				exit(4);
			}
			list = &list->children[1]->branch;
		}
		//tem mais literais do que declarados
		if(list != NULL){
			fprintf(stderr, "Lista %s tem mais filhos do que declarado.\n", identifier->key);
			exit(4);
		}
	break;
	case AST_DECLARACAO_FUN: 
		identifier->type = NAT_FUN | decl_type;
		list = identifier->param_list = (AstNode*) ast->children[2];
		while(list != NULL){
			//o nome do param já foi declarado
			if(list->children[1]->leaf.type){
				fprintf(stderr, "Parâmetro %s da função %s já foi declarado neste programa.\n",
					list->children[1]->leaf.key,
					identifier->key
				);
				exit(4);
			}
			list->children[1]->leaf.type = NAT_VAL | ast_type_to_type(list->children[0]->branch.type);
			//salva o ponteiro para o identificador da lista que está
			list->children[1]->leaf.param_list = identifier;
			list = (AstNode*) list->children[2];
		}

	break;
	default: printf("AST de tipo: %d é inválida!\n", ast->type); exit(5); break;
	}
}

void validate_function_args(AstNode* exp_list, AstNode* param_list){
	HashNode* fn_identifier = (HashNode*) exp_list->children[0];
	AstNode* exp = (AstNode*) exp_list->children[1];
	// tipo TK_IDENTIFIER paramListaContinua
	if(param_list == NULL){
		//NOTE: do jeito que a linguagem está feita, pode ser declaradas funções sem argumento, mas n pode chamá-las
		fprintf(stderr, "função %s não tem argumentos declarados.\n", fn_identifier->key);
		exit(4);
	}
	exp_list = (AstNode*) exp_list->children[2];
	while(param_list != NULL){
		parse_expression(exp, ast_type_to_type(param_list->children[0]->branch.type), fn_identifier);
		param_list = (AstNode*) param_list->children[2];
		if(param_list == NULL && exp_list != NULL){
			fprintf(stderr, "função %s foi chamada com mais argumentos do que esperado.\n", fn_identifier->key);
			exit(4);
		}
		if(exp_list == NULL){
			if(param_list == NULL) break;
			fprintf(stderr, "função %s foi chamada com menos argumentos do que esperado.\n", fn_identifier->key);
			exit(4);
		} 
		exp = (AstNode*) exp_list->children[0];
		exp_list = (AstNode*) exp_list->children[1];
	}
}
int _parse_expression(AstNode* exp, HashNode* fn_identifier){
	int left, right;
	switch (exp->type){
	case AST_EXP_IDENTIFIER: 
		if(!exp->children[0]->leaf.type){
			fprintf(stderr, "identificador %s não foi declarado neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		if(GET_NAT(((HashNode*)exp->children[0])) != NAT_VAL){
			fprintf(stderr, "identificador %s não foi declarado como escalar neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		//TODO: ver se a função atual é a função para qual o identificador foi declarado
		if(exp->children[0]->leaf.param_list != NULL){
			if(exp->children[0]->leaf.param_list != fn_identifier){
				fprintf(stderr, "identificador %s em %s foi declarado para o escopo de %s.\n",
					exp->children[0]->leaf.key,
					fn_identifier->key,
					((HashNode*) exp->children[0]->leaf.param_list)->key
				);
				exit(4);
			}
		}
		return GET_TYPE(((HashNode*) exp->children[0]));
	case AST_EXP_LITERAL:
		return hash_type_to_type(exp->children[0]->leaf.val);
	case AST_EXP_ARRAY_ACESS:
		if(!exp->children[0]->leaf.type){
			fprintf(stderr, "identificador %s não foi declarado neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		if(GET_NAT(((HashNode*)exp->children[0])) != NAT_VEC){
			fprintf(stderr, "identificador %s não foi declarado como vetor neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		left = _parse_expression((AstNode*) exp->children[1], fn_identifier);
		if(left != TYPE_INTE){
			fprintf(stderr, "expressão de acesso ao vetor %s precisa ser do tipo inteiro.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		return GET_TYPE(((HashNode*) exp->children[0]));
	case AST_EXP_FUN_CALL:
		if(!exp->children[0]->leaf.type){
			fprintf(stderr, "identificador %s não foi declarado neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		if(GET_NAT(((HashNode*)exp->children[0])) != NAT_FUN){
			fprintf(stderr, "identificador %s não foi declarado como função neste programa.\n", exp->children[0]->leaf.key);
			exit(4);
		}
		validate_function_args(exp, (AstNode*) exp->children[0]->leaf.param_list);
		return GET_TYPE(((HashNode*) exp->children[0]));
	case AST_EXP_INPUT:
		return ast_type_to_type(exp->children[0]->branch.type);
	case AST_EXP_SUM:
	case AST_EXP_SUB:
	case AST_EXP_MUL:
	case AST_EXP_DIV:
		left = _parse_expression((AstNode*) exp->children[0], fn_identifier);
		if(left == -1 || left == TYPE_BOOL) return -1;
		right = _parse_expression((AstNode*) exp->children[1], fn_identifier);
		if(right == -1 || left == TYPE_BOOL) return -1;
		return (left == right) ? left : -1;	
	case AST_EXP_LESS:
	case AST_EXP_GREAT:
	case AST_EXP_LE:
	case AST_EXP_GE:
	case AST_EXP_EQ:
	case AST_EXP_DIF:
		left = _parse_expression((AstNode*) exp->children[0], fn_identifier);
		if(left == -1 || left == TYPE_BOOL) return -1;
		right = _parse_expression((AstNode*) exp->children[1], fn_identifier);
		if(right == -1 || left == TYPE_BOOL) return -1;
		return (left == right) ? TYPE_BOOL : -1;
	case AST_EXP_AND:
	case AST_EXP_OR:
		left = _parse_expression((AstNode*) exp->children[0], fn_identifier);
		right = _parse_expression((AstNode*) exp->children[1], fn_identifier);
		return ((left == TYPE_BOOL) && (right == TYPE_BOOL)) ? TYPE_BOOL : -1;
	case AST_EXP_NEG:
		left = _parse_expression((AstNode*) exp->children[0], fn_identifier);
		return left == TYPE_BOOL ? TYPE_BOOL : -1;
	default: 
		printf("expressão %d >> 7 = %d não identificada\n", exp->type, exp->type >> 7); 
		exit(5); 
		break;
	}
}

void parse_expression(AstNode* exp, int expected_type, HashNode* fn_identifier){
	int solved_type = _parse_expression(exp, fn_identifier);
	if(solved_type == -1){
		fprintf(stderr, "expressão na função %s possui tipo incompatível.\n", fn_identifier->key);
		exit(4);
	}
	if(expected_type == -1) return;
	if(solved_type != expected_type){
		fprintf(stderr, "expressão na função %s possui tipo incompatível.\n\tExperava-se tipo %s obteve-se %s\n.",
			fn_identifier->key, type_to_string(expected_type), type_to_string(solved_type));
		exit(4);
	}
}

void parse_start_cmds(AstNode* start_cmd, HashNode* fn_identifier){
	switch (start_cmd->type){
		case AST_START_CMD_BLOCK: parse_cmds((AstNode*) start_cmd->children[0], fn_identifier); break;
		case AST_START_CMD_ACTION:parse_action((AstNode*) start_cmd->children[0], fn_identifier); break;
		case AST_START_CMD_FLOW:  parse_flow((AstNode*) start_cmd->children[0], fn_identifier); break;
		default: exit(5); break;
	}
}

void parse_cmds(AstNode* cmd, HashNode* fn_identifier){
	if(cmd == NULL) return;
	switch (cmd->type){
		case AST_CMD_BLOCK: parse_cmds((AstNode*) cmd->children[0], fn_identifier); break;
		case AST_CMD_ACTION: parse_action((AstNode*) cmd->children[0], fn_identifier); break;
		case AST_CMD_FLOW: parse_flow((AstNode*) cmd->children[0], fn_identifier); break;
		default: exit(5); break;
	}
	parse_cmds((AstNode*) cmd->children[1], fn_identifier);
}
void parse_flow(AstNode* flow, HashNode* fn_identifier){
	parse_expression((AstNode*) flow->children[0], TYPE_BOOL, fn_identifier);
	parse_start_cmds((AstNode*) flow->children[1], fn_identifier);
	if(flow->type ==  AST_FLOW_ELSEIF) {
		parse_start_cmds((AstNode*) flow->children[2], fn_identifier);
	}
}
void parse_action(AstNode* action, HashNode* fn_identifier){
	switch (action->type){
	case AST_ACTION_ASSIGN:
		if(!action->children[0]->leaf.type){
			fprintf(stderr, "identificador %s não foi declarado neste programa.\n", action->children[0]->leaf.key);
			exit(4);
		}
		if(GET_NAT(((HashNode*)action->children[0])) != NAT_VAL){
			fprintf(stderr, "identificador %s não foi declarado como escalar neste programa.\n", action->children[0]->leaf.key);
			exit(4);
		}
		//TODO: ver se a função atual é a função para qual o identificador foi declarado
		if(action->children[0]->leaf.param_list != NULL){
			if(action->children[0]->leaf.param_list != fn_identifier){
				fprintf(stderr, "identificador %s em %s foi declarado para o escopo de %s.\n",
					action->children[0]->leaf.key,
					fn_identifier->key,
					((HashNode*) action->children[0]->leaf.param_list)->key
				);
			}
		}
		parse_expression((AstNode*) action->children[1], GET_TYPE(((HashNode*) action->children[0])), fn_identifier);
		break;
	case AST_ACTION_ASSIGN_ARRAY:
		if(!action->children[0]->leaf.type){
			fprintf(stderr, "identificador %s não foi declarado neste programa.\n", action->children[0]->leaf.key);
			exit(4);
		}
		if(GET_NAT(((HashNode*)action->children[0])) != NAT_VEC){
			fprintf(stderr, "identificador %s não foi declarado como vetor neste programa.\n", action->children[0]->leaf.key);
			exit(4);
		}
		parse_expression((AstNode*) action->children[1], TYPE_INTE, fn_identifier);
		parse_expression((AstNode*) action->children[2], GET_TYPE(((HashNode*) action->children[0])), fn_identifier);
		break;
	case AST_ACTION_RETURN:
		parse_expression((AstNode*) action->children[0], GET_TYPE(fn_identifier), fn_identifier);
		break;
	case AST_ACTION_PRINT_EXP: 
		//print can be any type as long as it matches
		parse_expression((AstNode*) action->children[0], -1, fn_identifier);
		break;
	case AST_ACTION_PRINT_STRING: break;
	case AST_SEMICOLON: break;
	default: exit(5); break;
	}
}

void parse_function(AstNode* ast){
	HashNode* fn_identifier = &ast->children[0]->leaf;
	AstNode* start_cmd = (AstNode*) ast->children[1];
	//não foi declarado
	if(!fn_identifier->type){
		fprintf(stderr, "identificador %s não foi declarado neste programa.\n", fn_identifier->key);
		exit(4);
	}else if(GET_NAT(fn_identifier) != NAT_FUN){
		fprintf(stderr, "identificador %s não foi declarado como função neste programa.\n", fn_identifier->key);
		exit(4);
	}
	fn_identifier->type |= FUN_IMPLEMENTED;
	parse_start_cmds(start_cmd, fn_identifier);
}

void declaration_pass(AstNode* ast){
	// o primeiro filho de uma arvore n nula são as declarações
	ast = (AstNode*) ast->children[0]; 
	//parse da primeira declaração
	parse_declaration(&ast->children[0]->branch);
	while(ast->type != AST_DEF){
		ast = (AstNode*) ast->children[1];
		parse_declaration(&ast->children[0]->branch);
	}
}

void expressions_pass(AstNode* ast){
	if(ast->children[1] == NULL) return;

	ast = (AstNode*) ast->children[1]; 
	//parse da primeira declaração
	parse_function(&ast->children[0]->branch);
	while(ast->type != AST_FUN){
		ast = (AstNode*) ast->children[1];
		parse_function(&ast->children[0]->branch);
	}
}

void semantic_pass(){
	if(g_syntax_tree == NULL) return; 
	declaration_pass(g_syntax_tree);
	expressions_pass(g_syntax_tree);
	//jeito toscão de fazer
	HashNode* list;
	for(int i = 0; i < g_table->size; i++){
		list = g_table->elems[i]; 
		while(list != NULL){
			if(GET_NAT(list) == NAT_FUN && !WAS_IMPLEMENTED(list)){
				printf("função %s não foi implementada.\n", list->key);
				exit(4);
			}
			list = list->next;
		}
	}
}
