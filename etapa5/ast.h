#ifndef AST
#define AST

#define NPAD 3
#define LPAD 7

#define LAYOUT(a, b, c, d) (((d << 3) | (c << 2) | (b << 1) | (a)) << NPAD)

#define FIRST3BITS 7
#define AMOUNT(n) (n & FIRST3BITS)
//verifica se o bit i do segmento do layout é 0 ou 1, se for 0, a expressão resulta em 0 (false)
//se não ela resulta em um número > 0 (true)
#define ISTREE(t, i) (t & (1 << (i + NPAD))) 

/*
* cada ID é um valor único que contem 3 elementos
* os 3 primeiros bits indicam quantos elementos tem, de 0 a 4
* os 4 bits seguintes in indicam o layout desses filhos,
*	sendo 1 se um galho e 0 se é um folha
*   para os elementos do layout que não representam filhos usa-se 0, mas é irrelevante 
* os bits seguintes são usados para diferenciar os defines, pois há mais de um nó com a mesma configuração
* os valores são gerados no arquivo index.js, fazendo parse do parser.y
*/
#define AST_INICIO ((1 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_INICIO_AND ((2 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_DEF ((3 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_DEF_AND ((4 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_FUN ((5 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_FUN_AND ((6 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_KW_INT ((7 << LPAD) | LAYOUT(0, 0, 0, 0) | 0)
#define AST_KW_CHAR ((8 << LPAD) | LAYOUT(0, 0, 0, 0) | 0)
#define AST_KW_FLOAT ((9 << LPAD) | LAYOUT(0, 0, 0, 0) | 0)
#define AST_DECLARACAO ((10 << LPAD) | LAYOUT(1, 0, 0, 0) | 3)
#define AST_DECLARACAO_ARRAY ((11 << LPAD) | LAYOUT(1, 0, 0, 1) | 4)
#define AST_DECLARACAO_FUN ((12 << LPAD) | LAYOUT(1, 0, 1, 0) | 3)
#define AST_VALOR_LISTA ((13 << LPAD) | LAYOUT(0, 1, 0, 0) | 2)
#define AST_PARAM_LISTA ((14 << LPAD) | LAYOUT(1, 0, 1, 0) | 3)
#define AST_PARAM_LISTA_CONTINUA ((15 << LPAD) | LAYOUT(1, 0, 1, 0) | 3)
#define AST_EXP_IDENTIFIER ((16 << LPAD) | LAYOUT(0, 0, 0, 0) | 1)
#define AST_EXP_LITERAL ((17 << LPAD) | LAYOUT(0, 0, 0, 0) | 1)
#define AST_EXP_ARRAY_ACESS ((18 << LPAD) | LAYOUT(0, 1, 0, 0) | 2)
#define AST_EXP_CALL_FUN ((19 << LPAD) | LAYOUT(0, 1, 0, 0) | 2)
#define AST_EXP_INPUT ((20 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_EXP_NEG ((21 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_EXP_SUM ((22 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_SUB ((23 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_MUL ((24 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_DIV ((25 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_LESS ((26 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_GREAT ((27 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_LE ((28 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_GE ((29 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_EQ ((30 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_DIF ((31 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_AND ((32 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_EXP_OR ((33 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_CALL_FUN_PARAM ((34 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_CALL_FUN_PARAM_CONTINUA ((35 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_CODE ((36 << LPAD) | LAYOUT(0, 1, 0, 0) | 2)
#define AST_START_CMD_BLOCK ((37 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_START_CMD_ACTION ((38 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_START_CMD_FLOW ((39 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_CMD_BLOCK ((40 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_CMD_ACTION ((41 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_CMD_FLOW ((42 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_FLOW_WHILE ((43 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_FLOW_IF ((44 << LPAD) | LAYOUT(1, 1, 0, 0) | 2)
#define AST_FLOW_ELSEIF ((45 << LPAD) | LAYOUT(1, 1, 1, 0) | 3)
#define AST_ACTION_ASSIGN ((46 << LPAD) | LAYOUT(0, 1, 0, 0) | 2)
#define AST_ACTION_ASSIGN_ARRAY ((47 << LPAD) | LAYOUT(0, 1, 1, 0) | 3)
#define AST_ACTION_RETURN ((48 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_ACTION_PRINT_EXP ((49 << LPAD) | LAYOUT(1, 0, 0, 0) | 1)
#define AST_ACTION_PRINT_STRING ((50 << LPAD) | LAYOUT(0, 0, 0, 0) | 1)
#define AST_SEMICOLON ((51 << LPAD) | LAYOUT(0, 0, 0, 0) | 0)

#include <stdio.h>
#include "hashtable.h"

#define MAX_CHILDREN 4

union node_type;

typedef struct ast_node{
	int type;
	int line_number;
	char data_type;
	char data_nat;
	union node_type* children[MAX_CHILDREN]; 
} AstNode;

typedef union node_type{
	AstNode branch;
	HashNode leaf;
}NodeType;

AstNode* build_ast(int type, void* c0, void* c1, void* c2, void* c3);
void print_ast(FILE* file, AstNode* node);
void print_rebuild_file(FILE* file, AstNode* node);

#define NULL_FILL(...) _NULL_FILL(__VA_ARGS__ , NULL, NULL, NULL, NULL)
#define _NULL_FILL(a, b, c, d, ...) a, b, c, d

#define TNODE(t, ...) build_ast(t, NULL_FILL(__VA_ARGS__))
#define TTEND(t) build_ast(t, NULL, NULL, NULL, NULL);

#endif
