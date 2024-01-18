#include "tac.h"
#include <stdlib.h>
#include <stdio.h>

extern HashTable* g_table;
#define TEMPSYMBOL make_temp_symbol(g_table)
#define TEMPLABEL make_label(g_table)


Tac* make_tac(char type, HashNode* out, HashNode* op1, HashNode* op2){
	Tac *t = (Tac*) malloc(sizeof(Tac));
	t->type = type;
	t->out = out;
	t->op1 = op1;
	t->op2 = op2;
	t->prev = NULL;
	t->next = NULL;	
	return t;
}

Tac* join_tac(Tac* t1, Tac* t2){
	if(!t1) return t2;
	if(!t2) return t1;
	
    Tac* tac = t2;
	//retorna a lista de Tacs até o primeiro elemento de t2
    while(tac->prev){
		tac = tac->prev;	
	}
    //coloca o anterior de t2 como t1.
	tac->prev = t1;
    //retorna t1 concat t2
    //o pointer a t2 é para o fim da lista
	return t2;
}


Tac* reverse_tac(Tac* root_tac){
    Tac* t;
	for(t = root_tac; t->prev; t = t->prev)
		t->prev->next = t;
	return t;
}

void _print_tac(Tac* tac){
    if(tac->type == TAC_SYMBOL)return;

    fprintf(stderr, "TAC(");

    #define PRINT(x) case x: fprintf(stderr, # x);

    switch (tac->type){
    PRINT(TAC_MOVE); break;
    PRINT(TAC_ADD); break;
    PRINT(TAC_SUB); break;
    PRINT(TAC_MUL); break;
    PRINT(TAC_DIV); break;
    PRINT(TAC_LESS); break;
    PRINT(TAC_GREAT); break;
    PRINT(TAC_LE); break;
    PRINT(TAC_GE); break;
    PRINT(TAC_EQ); break;
    PRINT(TAC_DIF); break;
    PRINT(TAC_AND); break;
    PRINT(TAC_OR); break;
    PRINT(TAC_NEG); break;
    PRINT(TAC_LABEL); break;
    PRINT(TAC_BEGINFUN); break;
    PRINT(TAC_ENDFUN); break;
    PRINT(TAC_IFZ); break;
    PRINT(TAC_JUMP); break;
    PRINT(TAC_CALL); break;
    PRINT(TAC_ARG); break;
    PRINT(TAC_RET); break;
    PRINT(TAC_PRINT); break;
    PRINT(TAC_READ); break;
    default:
        fprintf(stderr, "TAC não detectada");
        break;
    }
    fprintf(stderr, "(");
    fprintf(stderr, "%s, ", tac->out ? tac->out->key : "NONE");
    fprintf(stderr, "%s, ", tac->op1 ? tac->op1->key : "NONE");
    fprintf(stderr, "%s))\n", tac->op2 ? tac->op2->key : "NONE");
}

void print_tac(Tac* tac){
	for(Tac* t = tac; t; t = t->next) _print_tac(t);	
}

Tac* make_binop_tac(char type, Tac* c[]){
    return join_tac(c[0], join_tac(c[1],
        make_tac(type, TEMPSYMBOL, 
            c[0] ? c[0]->out : NULL,
            c[1] ? c[1]->out : NULL
        )
    ));
}


Tac* _build_tac(AstNode* ast){
    if(!ast) return NULL;

    Tac* tac_children[MAX_CHILDREN];
    
    for(int i = 0; i < AMOUNT(ast->type); i++){
		if(ISTREE(ast->type, i)){
			tac_children[i] = _build_tac((AstNode*) ast->children[i]);
		}
	}

    switch (ast->type){
    case AST_KW_INT:
    case AST_KW_CHAR:
    case AST_KW_FLOAT:
        return NULL;

    case AST_EXP_IDENTIFIER:
    case AST_EXP_LITERAL: 
        return make_tac(TAC_SYMBOL, (HashNode*) ast->children[0], NULL, NULL);
    // case AST_EXP_ARRAY_ACESS: return;

    case AST_EXP_SUM: return make_binop_tac(TAC_ADD, tac_children);
    case AST_EXP_SUB: return make_binop_tac(TAC_SUB, tac_children);
    case AST_EXP_MUL: return make_binop_tac(TAC_MUL, tac_children);
    case AST_EXP_DIV: return make_binop_tac(TAC_DIV, tac_children);
    case AST_EXP_LESS: return make_binop_tac(TAC_LESS, tac_children);
    case AST_EXP_GREAT: return make_binop_tac(TAC_GREAT, tac_children);
    case AST_EXP_LE: return make_binop_tac(TAC_LE, tac_children);
    case AST_EXP_GE: return make_binop_tac(TAC_GE, tac_children);
    case AST_EXP_EQ: return make_binop_tac(TAC_EQ, tac_children);
    case AST_EXP_DIF: return make_binop_tac(TAC_DIF, tac_children);
    case AST_EXP_AND: return make_binop_tac(TAC_AND, tac_children);
    case AST_EXP_OR: return make_binop_tac(TAC_OR, tac_children);
    case AST_EXP_NEG: return make_binop_tac(TAC_NEG, tac_children);

    case AST_EXP_INPUT: return make_tac(TAC_READ, TEMPSYMBOL, NULL, NULL);
    
    case AST_EXP_CALL_FUN: return join_tac(tac_children[0], 
        make_tac(TAC_CALL, TEMPSYMBOL,(HashNode*) ast->children[0], NULL));
    case AST_CALL_FUN_PARAM:    
    case AST_CALL_FUN_PARAM_CONTINUA: 
        return join_tac(tac_children[1], join_tac(tac_children[0],
            make_tac(TAC_ARG, tac_children[0]->out, NULL, NULL)
        ));
    
    // case AST_CODE: return ;
    // case AST_FLOW_WHILE: return ;
    // case AST_FLOW_IF: return ;
    // case AST_FLOW_ELSEIF: return ;
    // case AST_ACTION_ASSIGN: return ;
    // case AST_ACTION_ASSIGN_ARRAY: return ;
    // case AST_ACTION_RETURN: return ;
    // case AST_ACTION_PRINT_EXP: return ;
    // case AST_ACTION_PRINT_STRING: return ;
        
    default: return join_tac(
        join_tac(join_tac(tac_children[0], tac_children[1]), 
            tac_children[2]),
        tac_children[3]
    );
    }
}

Tac* build_tac(AstNode* ast){
    return _build_tac(ast);
}
