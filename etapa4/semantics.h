#ifndef SEMANTICS
#define SEMANTICS

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "hashtable.h"

#define TYPE_LAYOUT_OFFSET 2
#define NAT_LAYOUT_OFFSET (TYPE_LAYOUT_OFFSET + 2)

#define TYPE_BOOL 3
#define TYPE_INTE 2
#define TYPE_REAL 1

#define NAT_VAL (1 << TYPE_LAYOUT_OFFSET)
#define NAT_FUN (2 << TYPE_LAYOUT_OFFSET)
#define NAT_VEC (3 << TYPE_LAYOUT_OFFSET)

#define FUN_IMPLEMENTED (1 << NAT_LAYOUT_OFFSET)

#define GET_TYPE(x) (x->type & 3)
#define GET_NAT(x) (x->type & (3 << TYPE_LAYOUT_OFFSET))
#define WAS_IMPLEMENTED(x) (x->type & (1 << NAT_LAYOUT_OFFSET))

void semantic_pass();

#endif
