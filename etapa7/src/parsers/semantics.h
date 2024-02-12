#ifndef SEMANTICS
#define SEMANTICS

#include <stdio.h>
#include <stdlib.h>

#include "../data_structures/ast.h"
#include "../data_structures/hashtable.h"

#define TYPE_REAL 1
#define TYPE_INTE 2
#define TYPE_BOOL 3

#define NAT_VAL 4
#define NAT_FUN 5
#define NAT_VEC 6

void semantic_pass(void);

#endif
