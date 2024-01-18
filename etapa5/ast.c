#include "ast.h"

#include <stdlib.h>

//declado em scanner.l
extern int getLineNumber(void);


AstNode* build_ast(int type, void* c0, void* c1, void* c2, void* c3){
	AstNode* node = (AstNode*) malloc(sizeof(AstNode));
	node->type = type;
	node->line_number = getLineNumber();
	node->data_type = 0;
	node->data_nat = 0;
	node->children[0] = c0;
	node->children[1] = c1;
	node->children[2] = c2;
	node->children[3] = c3;

	return node;
}

void _print_ast(FILE* file, AstNode* node, int depth){
	if(node == NULL) return;
	int i = depth;
	fprintf(file, "%d.", node->line_number);
	while(i--) fprintf(file, " ");

	#define TPRINT(t) case t: fprintf(file, # t); break
	switch (node->type){
		TPRINT(AST_INICIO);
		TPRINT(AST_INICIO_AND);
		TPRINT(AST_DEF);
		TPRINT(AST_DEF_AND);
		TPRINT(AST_FUN);
		TPRINT(AST_FUN_AND);
		TPRINT(AST_KW_INT);
		TPRINT(AST_KW_CHAR);
		TPRINT(AST_KW_FLOAT);
		TPRINT(AST_DECLARACAO);
		TPRINT(AST_DECLARACAO_ARRAY);
		TPRINT(AST_DECLARACAO_FUN);
		TPRINT(AST_VALOR_LISTA);
		TPRINT(AST_PARAM_LISTA);
		TPRINT(AST_PARAM_LISTA_CONTINUA);
		TPRINT(AST_EXP_ARRAY_ACESS);
		TPRINT(AST_EXP_CALL_FUN);
		TPRINT(AST_EXP_INPUT);
		TPRINT(AST_EXP_NEG);
		TPRINT(AST_EXP_SUM);
		TPRINT(AST_EXP_SUB);
		TPRINT(AST_EXP_MUL);
		TPRINT(AST_EXP_DIV);
		TPRINT(AST_EXP_LESS);
		TPRINT(AST_EXP_GREAT);
		TPRINT(AST_EXP_LE);
		TPRINT(AST_EXP_GE);
		TPRINT(AST_EXP_EQ);
		TPRINT(AST_EXP_DIF);
		TPRINT(AST_EXP_AND);
		TPRINT(AST_EXP_OR);
		TPRINT(AST_CALL_FUN_PARAM);
		TPRINT(AST_CALL_FUN_PARAM_CONTINUA);
		TPRINT(AST_CODE);
		TPRINT(AST_START_CMD_BLOCK);
		TPRINT(AST_START_CMD_ACTION);
		TPRINT(AST_START_CMD_FLOW);
		TPRINT(AST_CMD_BLOCK);
		TPRINT(AST_CMD_ACTION);
		TPRINT(AST_CMD_FLOW);
		TPRINT(AST_FLOW_WHILE);
		TPRINT(AST_FLOW_IF);
		TPRINT(AST_FLOW_ELSEIF);
		TPRINT(AST_ACTION_ASSIGN);
		TPRINT(AST_ACTION_ASSIGN_ARRAY);
		TPRINT(AST_ACTION_RETURN);
		TPRINT(AST_ACTION_PRINT_EXP);
		TPRINT(AST_ACTION_PRINT_STRING);
		TPRINT(AST_SEMICOLON);
		TPRINT(AST_EXP_IDENTIFIER);
        TPRINT(AST_EXP_LITERAL);
		default: fprintf(file, "Type: %d", node->type); break;
	}
	fprintf(file, "\n");

	for(int j = 0; j < AMOUNT(node->type); j++){
		if(ISTREE(node->type, j)){
			_print_ast(file, &node->children[j]->branch, depth + 1);
		}else{
			i = depth + 1;
			while(i--) fprintf(file, " ");
			fprintf(file, "%s\n", node->children[j]->leaf.key);
		}
	}
}

void print_ast(FILE* file, AstNode* node){ 
	fprintf(stderr, "AST:\n");
	_print_ast(file, node, 0);
} 

void print_rebuild_file(FILE* file, AstNode* node){
	if(node == NULL) return;

	#define REC(n) print_rebuild_file(file, n)

	switch (node->type){
	case AST_INICIO:
		REC(&node->children[0]->branch);
		break;
	case AST_INICIO_AND:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_DEF:
		REC(&node->children[0]->branch);
		break;
	case AST_DEF_AND:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_FUN:
		REC(&node->children[0]->branch);
		break;
	case AST_FUN_AND:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_KW_INT:
		fprintf(file, "int ");
		break;
	case AST_KW_CHAR:
		fprintf(file, "char ");
		break;
	case AST_KW_FLOAT:
		fprintf(file, "float ");
		break;
	case AST_DECLARACAO:
		REC(&node->children[0]->branch);
		fprintf(file, "%s = %s;\n",
			node->children[1]->leaf.key,
			node->children[2]->leaf.key
		);
		break;
	case AST_DECLARACAO_ARRAY:
		REC(&node->children[0]->branch);
		fprintf(file, "%s[%s] ",
			node->children[1]->leaf.key,
		 	node->children[2]->leaf.key
		);
		REC(&node->children[3]->branch);
		fprintf(file, ";\n");
		break;
	case AST_DECLARACAO_FUN:
		REC(&node->children[0]->branch);
		fprintf(file, "%s( ",
			node->children[1]->leaf.key
		);
		REC(&node->children[2]->branch);
		fprintf(file, ");\n");
		break;
	case AST_VALOR_LISTA:
		fprintf(file, "%s ", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		break;
	case AST_PARAM_LISTA:
		REC(&node->children[0]->branch);
		fprintf(file, "%s ", node->children[1]->leaf.key);
		REC(&node->children[2]->branch);
		break;
	case AST_PARAM_LISTA_CONTINUA:
		fprintf(file, ", ");
		REC(&node->children[0]->branch);
		fprintf(file, "%s ", node->children[1]->leaf.key);
		REC(&node->children[2]->branch);
		break;
	case AST_EXP_IDENTIFIER:
		fprintf(file, "%s ", node->children[0]->leaf.key);
		break;
	case AST_EXP_LITERAL:
		fprintf(file, "%s ", node->children[0]->leaf.key);
		break;
	case AST_EXP_ARRAY_ACESS:
		fprintf(file, "%s[", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		fprintf(file, "] ");
		break;
	case AST_EXP_CALL_FUN:
		fprintf(file, "%s(", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		fprintf(file, ") ");
		break;
	case AST_EXP_INPUT:
		fprintf(file, "input(");
		REC(&node->children[0]->branch);
		fprintf(file, ") ");
		break;
	case AST_EXP_NEG:
		fprintf(file, "(~");
		REC(&node->children[0]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_SUM:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " + ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_SUB:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " - ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_MUL:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " * ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_DIV:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " / ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_LESS:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " < ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_GREAT:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " > ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_LE:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " <= ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_GE:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " >= ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_EQ:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " == ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_DIF:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " != ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_AND:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " & ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_EXP_OR:
		fprintf(file, "(");
		REC(&node->children[0]->branch);
		fprintf(file, " | ");
		REC(&node->children[1]->branch);
		fprintf(file, ")");
		break;
	case AST_CALL_FUN_PARAM:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_CALL_FUN_PARAM_CONTINUA:
		fprintf(file, ", ");
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_CODE:
		fprintf(file, "code %s ", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		break;
	case AST_START_CMD_BLOCK:
		fprintf(file, "{\n");
		REC(&node->children[0]->branch);
		fprintf(file, "}\n");
		break;
	case AST_START_CMD_ACTION:
		REC(&node->children[0]->branch);
		break;
	case AST_START_CMD_FLOW:
		REC(&node->children[0]->branch);
		break;
	case AST_CMD_BLOCK:
		fprintf(file, "{\n");
		REC(&node->children[0]->branch);
		fprintf(file, "}\n");
		REC(&node->children[1]->branch);
		break;
	case AST_CMD_ACTION:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_CMD_FLOW:
		REC(&node->children[0]->branch);
		REC(&node->children[1]->branch);
		break;
	case AST_FLOW_WHILE:
		fprintf(file, "while( ");
		REC(&node->children[0]->branch);
		fprintf(file, " ) ");
		REC(&node->children[1]->branch);
		break;
	case AST_FLOW_IF:
		fprintf(file, "if( ");
		REC(&node->children[0]->branch);
		fprintf(file, " ) ");
		REC(&node->children[1]->branch);
		break;
	case AST_FLOW_ELSEIF:
		fprintf(file, "if( ");
		REC(&node->children[0]->branch);
		fprintf(file, " ) ");
		REC(&node->children[1]->branch);
		fprintf(file, " else ");
		REC(&node->children[2]->branch);
		break;
	case AST_ACTION_ASSIGN:
		fprintf(file, "%s = ", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		fprintf(file, ";\n");
		break;
	case AST_ACTION_ASSIGN_ARRAY:
		fprintf(file, "%s[ ", node->children[0]->leaf.key);
		REC(&node->children[1]->branch);
		fprintf(file, "] = ");
		REC(&node->children[2]->branch);
		fprintf(file, ";\n");
		break;
	case AST_ACTION_RETURN:
		fprintf(file, "return ");
		REC(&node->children[0]->branch);
		fprintf(file, ";\n");
		break;
	case AST_ACTION_PRINT_EXP:
		fprintf(file, "print ");
		REC(&node->children[0]->branch);
		fprintf(file, ";\n");
		break;
	case AST_ACTION_PRINT_STRING:
		fprintf(file, "print %s;\n", node->children[0]->leaf.key);
		break;
	case AST_SEMICOLON:
		fprintf(file, ";");
		break;
	default:
		fprintf(file, "ERROR!!");
		break;
	}

}
