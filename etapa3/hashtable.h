#ifndef HASH_TABLE
#define HASH_TABLE

/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

typedef struct hash_node{
	char* key;
	int val;
	struct hash_node* next;
} HashNode;
 
typedef struct hash_table{
	int size;
	int ocupation;
	HashNode** elems;
} HashTable;

//public
HashTable* create_table(int size);
HashNode* insert_table(HashTable* t, char* key, int val);
//HashNode* get_table(HashTable* t, char* key);
void print_table(HashTable* t);


//private
/*
HashTable* _rehash(HashTable* t);
int _calc_hash(HashTable* t, char* key);
HashNode* _find_node(HashTable* t, char* key);
*/

#endif
