#ifndef TAC_HEADER
#define TAC_HEADER


#define TAC_SYMBOL 1
#define TAC_MOVE 2
#define TAC_ADD 3
#define TAC_SUB 4
#define TAC_MUL 5
#define TAC_DIV 6
#define TAC_LESS 7
#define TAC_GREAT 8
#define TAC_LE 9
#define TAC_GE 10
#define TAC_EQ 11
#define TAC_DIF 12
#define TAC_AND 13
#define TAC_OR 14
#define TAC_NEG 15
#define TAC_LABEL 16
#define TAC_BEGINFUN 17
#define TAC_ENDFUN 18
#define TAC_IFZ 19
#define TAC_JUMP 20
#define TAC_CALL 21
#define TAC_ARG 22
#define TAC_RET 23
#define TAC_PRINT 24
#define TAC_READ 25
#define TAC_ACESS_VEC 26
#define TAC_MOVE_VEC 27

#include "hashtable.h"
#include "ast.h"


typedef struct tac_t{
    char type;
    HashNode* out;
    HashNode* op1;
    HashNode* op2;
    struct tac_t* next;
    struct tac_t* prev;
}Tac;


// os TACs devem
// - criar
// - imprimir
// - unir listas de instruções

Tac* build_tac(AstNode* ast);
void print_tac(Tac* tac);
Tac* reverse_tac(Tac* tac);



#endif
