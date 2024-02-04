#ifndef BTREE
#define BTREE


typedef struct btree_t {
	char* key;
	char* val;
	struct btree_t* left; 
	struct btree_t* right; 
} Btree;


Btree* btree_create(char* key, char* val);

void btree_insert(Btree* tree, char* key, char* val);

//if the tree does not contain an alias, returns NULL
char* btree_get(Btree* tree, char* key);

//if the tree does not contain an alias, returns key
char* try_btree_get(Btree* tree, char* key);

int btree_height(Btree* tree);
void btree_print(Btree* tree); 

//TODO: implementar um free para eliminar os alias
void btree_free(Btree* tree); 

#endif
