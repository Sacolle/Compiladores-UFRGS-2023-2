#ifndef BTREE
#define BTREE



typedef struct btree_lifetime_node_t {
	char* key;
	char first_use;
	char last_use;
	struct btree_lifetime_node_t* left; 
	struct btree_lifetime_node_t* right; 
} LtMapNode;

typedef struct btree_lifetime_t {
	struct btree_lifetime_node_t* root; 
} LtMap;

LtMap* ltmap_create();
void ltmap_insert(LtMap* map, char* key, int first_use, int last_use);
//int ltmap_has(LtMap* map, char* key);
LtMapNode* ltmap_get(LtMap* map, char* key);
void ltmap_print(LtMap* map); 
void ltmap_free(LtMap* map); 


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
