#ifndef HASH_TABLE
#define HASH_TABLE

/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#define SYMBOL_LIT_REAL 1
#define SYMBOL_LIT_CARA 2
#define SYMBOL_LIT_INTE 3
#define SYMBOL_LIT_STRING 4
#define SYMBOL_IDENTIFIER 7
#define SYMBOL_TEMP_IDENTIFIER 8
#define SYMBOL_LABEL 9

// literal or identifier?
// nature: val, fn, vec
// type: int, char, float, string
// declared: first bit == 1

typedef struct hash_node{
	char* key;
	int val;
	char type;
	char nature;
	char is_implemented;
	void* param_list;	//pointer para a AST
	int num_of_local_var;
	int inlinable;
	struct hash_node* next;
} HashNode;
 
typedef struct hash_table{
	int size;
	int ocupation;
	HashNode** elems;
} HashTable;

//adicionar 2 funções na tabela hash
// - De criar um simbolo temporário (makeTemp)
// - De criar labels (makeLabel)

//public
HashTable* create_table(int size);
HashNode* insert_table(HashTable* t, char* key, int val);
HashNode* make_temp_symbol(HashTable* t);
HashNode* make_label(HashTable* t);
//HashNode* get_table(HashTable* t, char* key);
void print_table(HashTable* t);



//private
/*
HashTable* _rehash(HashTable* t);
int _calc_hash(HashTable* t, char* key);
HashNode* _find_node(HashTable* t, char* key);
*/

#endif
