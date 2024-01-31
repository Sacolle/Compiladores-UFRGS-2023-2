
#include "semantics.h"

extern HashTable *g_table; 
extern AstNode *g_syntax_tree;

int g_error_counter = 0;
int g_line_number = 0;

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
/*
void parse_start_cmds(AstNode* start_cmd, HashNode* fn_identifier);
void parse_declaration(AstNode* ast);
void validate_function_args(AstNode* exp, AstNode* param_list);
int _parse_expression(AstNode* exp, HashNode* fn_identifier);
void parse_expression(AstNode* exp, int expected_type, HashNode* fn_identifier);
void parse_cmds(AstNode* cmd, HashNode* fn_identifier);
void parse_flow(AstNode* flow, HashNode* fn_identifier);
void parse_action(AstNode* action, HashNode* fn_identifier);
void parse_function(AstNode* ast);
*/

char* type_to_string(int t){
	switch (t){
		case TYPE_BOOL: return "bool";
		case TYPE_INTE: return "int";
		case TYPE_REAL: return "real";
		default: return "";
	}
}

int ast_type_to_type(int t){
	switch (t){
	case AST_KW_CHAR: 
	case AST_KW_INT: 
		return TYPE_INTE;
	case AST_KW_FLOAT: 
		return TYPE_REAL;
	default: printf("conversion failed, type received was %d >> 7 = %d", t, t >> 7); g_error_counter++; return 0;
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

//se type != 0, então o identifier já foi declarado e já tem tipo, então
//o g_error_counter é incrementado
int indentifier_not_declared(HashNode* identifier){
	if(identifier->type){
		fprintf(stderr, "LINHA %d: identificador %s já foi declarado neste programa.\n", 
			g_line_number, identifier->key
		);
		g_error_counter++;
		return 0;
	}else{
		return 1;
	}
}

void parse_AST_DECLARACAO(AstNode* ast){
	int type = ast_type_to_type(ast->children[0]->branch.type);
	HashNode* identifier = (HashNode*) ast->children[1];
	HashNode* literal = (HashNode*) ast->children[2];

	if(indentifier_not_declared(identifier)){
		identifier->type = type;
		identifier->nature = NAT_VAL;
	}

	if(hash_type_to_type(literal->val) != type){
		fprintf(stderr, "LINHA %d: literal %s e tipo da declaração são incompatíveis.\n",
			g_line_number, literal->key
		);
		g_error_counter++;
	}
}
void parse_AST_DECLARACAO_ARRAY(AstNode* ast){
	int type = ast_type_to_type(ast->children[0]->branch.type);
	int array_size = atoi(ast->children[2]->leaf.key);
	HashNode* identifier = (HashNode*) ast->children[1];
	AstNode* list = (AstNode*) ast->children[3];

	if(indentifier_not_declared(identifier)){
		identifier->type = type;
		identifier->nature = NAT_VEC;
	}
	if(list == NULL){
		return;
	}

	for(int i = 0; i < array_size; i++){
		if(hash_type_to_type(list->children[0]->leaf.val) != type){
			fprintf(stderr, "LINHA %d: literal %s e tipo declarado são incompatíveis.\n",
				g_line_number,
				list->children[0]->leaf.key
			);
			g_error_counter++;
		}
		// a lista tem menos filhos q declarado
		if((list->children[1] == NULL) && (i != array_size - 1)){
			fprintf(stderr, "LINHA %d: Lista %s tem menos filhos do que declarado.\n", g_line_number, identifier->key);
			g_error_counter++;
		}
		list = &list->children[1]->branch;
	}
	//tem mais literais do que declarados
	if(list != NULL){
		fprintf(stderr, "LINHA %d: Lista %s tem mais filhos do que declarado.\n", g_line_number, identifier->key);
		g_error_counter++;
	}
}
void parse_AST_DECLARACAO_FUN(AstNode* ast){
	int type = ast_type_to_type(ast->children[0]->branch.type);
	HashNode* identifier = (HashNode*) ast->children[1];

	if(indentifier_not_declared(identifier)){
		identifier->type = type;
		identifier->nature = NAT_FUN;
		identifier->param_list = (AstNode*) ast->children[2];
		identifier->is_implemented = 1;
	}
}
void parse_function_parameter_list(AstNode* ast){
	int type = ast_type_to_type(ast->children[0]->branch.type);
	HashNode* identifier = (HashNode*) ast->children[1];

	if(indentifier_not_declared(identifier)){
		identifier->type = type;
		identifier->nature = NAT_VAL;
	}
	ast->data_nat = NAT_VAL;
	ast->data_type = type;
}
void parse_AST_EXP_IDENTIFIER(AstNode* ast){
	HashNode* identifier = (HashNode*) ast->children[0];
	if(identifier->nature == 0){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}
	if(identifier->nature != NAT_VAL){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado como valor.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}
	ast->data_nat = NAT_VAL;
	ast->data_type = identifier->type;
}
void parse_AST_EXP_LITERAL(AstNode* ast){
	ast->data_nat = NAT_VAL;
	ast->data_type = hash_type_to_type(ast->children[0]->leaf.val);
}
void parse_AST_EXP_ARRAY_ACESS(AstNode* ast){
	HashNode* identifier = (HashNode*) ast->children[0];
	if(identifier->nature == 0){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}

	if(identifier->nature != NAT_VEC){
		fprintf(stderr, "LINHA %d: Não pode-se indexar o identificador %s, pois este não é um vetor.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}

	ast->data_nat = NAT_VAL;
	ast->data_type = ast->children[0]->leaf.type;
	
	if(ast->children[1]->branch.data_type != TYPE_INTE){
		fprintf(stderr, "LINHA %d: o valor de indexação deve ser do tipo inteiro.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(ast->children[1]->branch.data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: o token de indexação deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
}
//TODO: validar que isto está funcionando
void validate_function_args(HashNode* fn_identifier, AstNode* called_param, AstNode* declared_param){
	if(called_param != NULL && declared_param == NULL){
		fprintf(stderr, "LINHA %d: função %s foi chamada com mais argumentos do que esperado.\n", 
			g_line_number,
			fn_identifier->key
		);
		g_error_counter++;
		return;
	}
	if(called_param == NULL && declared_param != NULL){
		fprintf(stderr, "LINHA %d: função %s foi chamada com menos argumentos do que esperado.\n", 
			g_line_number,
			fn_identifier->key
		);
		g_error_counter++;
		return;
	}
	if(called_param == NULL && declared_param == NULL) return;

	if(called_param->data_type != declared_param->data_type){
		fprintf(stderr, "LINHA %d: argumento da função %s foi chamado com um tipo incompatível.\n",
			g_line_number,
			fn_identifier->key
		);
		g_error_counter++;
	}
	validate_function_args(fn_identifier,
		(AstNode*) called_param->children[1],
		(AstNode*) declared_param->children[2]
	);

}

void parse_AST_EXP_CALL_FUN(AstNode* ast){
	HashNode* identifier = (HashNode*) ast->children[0];
	if(identifier->nature == 0){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}
	if(identifier->nature != NAT_FUN){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado como função.\n",
			g_line_number,
			identifier->key
		);
		g_error_counter++;
	}
	ast->data_nat = NAT_VAL;
	ast->data_type = identifier->type;

	validate_function_args(identifier,(AstNode*) ast->children[1], identifier->param_list);
}

void parse_function_call_list(AstNode* ast){
	AstNode* exp = (AstNode*) ast->children[0];
	
	if(exp->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	ast->data_nat = NAT_VAL;
	ast->data_type = exp->data_type;
}

void parse_AST_EXP_INPUT(AstNode* ast){
	ast->data_nat = NAT_VAL;
	ast->data_type = ast_type_to_type(ast->children[0]->branch.type);
}
void parse_AST_EXP_NEG(AstNode* ast){
	AstNode* c1 = (AstNode*) ast->children[0];
	if(c1->data_type != TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão deve ser do tipo inteiro.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}

	ast->data_nat = NAT_VAL;
	ast->data_type = TYPE_BOOL;
}
void parse_ast_binop_num_to_num(AstNode* ast){
	AstNode* c1 = (AstNode*) ast->children[0];
	AstNode* c2 = (AstNode*) ast->children[1];

	if(c1->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_type == TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda não pode ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_type == TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da direita não pode ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_type != c2->data_type){
		fprintf(stderr, "LINHA %d: as expressão possuem tipos incompatíveis.\n",
			g_line_number
		);
		g_error_counter++;
	}

	ast->data_nat = NAT_VAL;
	ast->data_type = c1->data_type;
}
void parse_ast_binop_num_to_bool(AstNode* ast){
	AstNode* c1 = (AstNode*) ast->children[0];
	AstNode* c2 = (AstNode*) ast->children[1];

	if(c1->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_type == TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda não pode ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_type == TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da direita não pode ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_type != c2->data_type){
		fprintf(stderr, "LINHA %d: as expressão possuem tipos incompatíveis.\n",
			g_line_number
		);
		g_error_counter++;
	}

	ast->data_nat = NAT_VAL;
	ast->data_type = TYPE_BOOL;
}
void parse_ast_binop_bool_to_bool(AstNode* ast){
	AstNode* c1 = (AstNode*) ast->children[0];
	AstNode* c2 = (AstNode*) ast->children[1];

	if(c1->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_type != TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c2->data_type == TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser do tipo booleano.\n",
			g_line_number
		);
		g_error_counter++;
	}

	ast->data_nat = NAT_VAL;
	ast->data_type = TYPE_BOOL;
}
void parse_AST_CODE(AstNode* ast){
	HashNode* fun = (HashNode*) ast->children[0];
	if(fun->nature == 0){
		fprintf(stderr, "LINHA %d: Função %s não foi declarada.\n",
			g_line_number,
			fun->key
		);
		g_error_counter++;
	}
	if(fun->nature != NAT_FUN){
		fprintf(stderr, "LINHA %d: identificador %s não foi declarado como função.\n",
			g_line_number,
			fun->key
		);
		g_error_counter++;
	}
	fun->is_implemented = 1;
}
void parse_ast_flow(AstNode* ast){
	AstNode* c1 = (AstNode*) ast->children[0];
	if(c1->data_type != TYPE_BOOL){
		fprintf(stderr, "LINHA %d: a expressão deve ser do tipo inteiro.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(c1->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
}
void parse_AST_ACTION_ASSIGN(AstNode* ast){
	HashNode* identifier = (HashNode*) ast->children[0];
	AstNode* exp = (AstNode*) ast->children[1];

	if(identifier->nature != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(exp->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(identifier->type != exp->data_type){
		fprintf(stderr, "LINHA %d: as expressão possuem tipos incompatíveis.\n",
			g_line_number
		);
		g_error_counter++;
	}
}
void parse_AST_ACTION_ASSIGN_ARRAY(AstNode* ast){
	HashNode* identifier = (HashNode*) ast->children[0];
	AstNode* idx = (AstNode*) ast->children[1];
	AstNode* exp = (AstNode*) ast->children[2];

	if(identifier->nature != NAT_VEC){
		fprintf(stderr, "LINHA %d: a expressão da esquerda deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(idx->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: o índice deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(exp->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão da direita deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(idx->data_type != TYPE_INTE){
		fprintf(stderr, "LINHA %d: o índice deve do tipo inteiro.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(identifier->type != exp->data_type){
		fprintf(stderr, "LINHA %d: as expressão possuem tipos incompatíveis.\n",
			g_line_number
		);
		g_error_counter++;
	}
}
void parse_ast_return(AstNode* ast, HashNode* fun){
	AstNode* exp = (AstNode*) ast->children[0];

	if(exp->data_nat != NAT_VAL){
		fprintf(stderr, "LINHA %d: a expressão de retorno deve ser um valor.\n",
			g_line_number
		);
		g_error_counter++;
	}
	if(fun->type != exp->data_type){
		fprintf(stderr, "LINHA %d: o tipo de retorno é incompatível com a função.\n",
			g_line_number
		);
		g_error_counter++;
	}
}

void parse_tree(AstNode* ast, HashNode* fun){
	if(ast == NULL) return;

	for(int i = 0; i < AMOUNT(ast->type); i++){
		if(ISTREE(ast->type, i)){
			if(ast->type == AST_CODE){
				parse_tree((AstNode*) ast->children[i], (HashNode*) ast->children[0]);
			}else{
				parse_tree((AstNode*) ast->children[i], fun);
			}
		}
	}
	g_line_number = ast->line_number;

	#define PARSE(x) case x: parse_ ## x(ast);
	switch (ast->type){
		PARSE(AST_DECLARACAO); break;
		PARSE(AST_DECLARACAO_ARRAY); break;
		PARSE(AST_DECLARACAO_FUN); break;
		case AST_PARAM_LISTA:
		case AST_PARAM_LISTA_CONTINUA:
			parse_function_parameter_list(ast);
			break;
		PARSE(AST_EXP_IDENTIFIER); break;
		PARSE(AST_EXP_LITERAL); break;
		PARSE(AST_EXP_ARRAY_ACESS); break;
		PARSE(AST_EXP_CALL_FUN); break;
		PARSE(AST_EXP_INPUT); break;
		PARSE(AST_EXP_NEG); break;
		case AST_EXP_SUM:
		case AST_EXP_SUB:
		case AST_EXP_MUL:
		case AST_EXP_DIV:
			parse_ast_binop_num_to_num(ast);
			break;
		case AST_EXP_LESS:
		case AST_EXP_GREAT:
		case AST_EXP_LE:
		case AST_EXP_GE:
		case AST_EXP_EQ:
		case AST_EXP_DIF:
			parse_ast_binop_num_to_bool(ast);
			break;
		case AST_EXP_AND:
		case AST_EXP_OR:
			parse_ast_binop_bool_to_bool(ast);
			break;
		case AST_CALL_FUN_PARAM:
		case AST_CALL_FUN_PARAM_CONTINUA:
			parse_function_call_list(ast);
			break;
		PARSE(AST_CODE); break;
		case AST_FLOW_WHILE: 
		case AST_FLOW_IF: 
		case AST_FLOW_ELSEIF:
			parse_ast_flow(ast);
			break; 
		PARSE(AST_ACTION_ASSIGN); break;
		PARSE(AST_ACTION_ASSIGN_ARRAY); break;
		case AST_ACTION_RETURN: 
			parse_ast_return(ast, fun); 
			break;
	default:
		break;
	}
}

void semantic_pass(void){
	parse_tree(g_syntax_tree, NULL);
	//jeito toscão de fazer
	HashNode* list;
	for(int i = 0; i < g_table->size; i++){
		list = g_table->elems[i]; 
		while(list != NULL){
			if(list->nature == NAT_FUN && !list->is_implemented){
				printf("função %s não foi implementada.\n", list->key);
				g_error_counter++;
			}
			list = list->next;
		}
	}
}
