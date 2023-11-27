/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "hashtable.h"


int _calc_hash(HashTable* t, char* key){
	int idx = 1;
    for(int i = 0; i < strlen(key); i++){
        idx = (idx * key[i]) % t->size + 1;
    }
    return idx - 1;
}

HashNode* _find_node(HashTable* t, char* key){
	int indice = _calc_hash(t, key);

	HashNode* node = t->elems[indice];
	while(node != NULL){
		if(strcmp(node->key, key) == 0){
			return node;
		}
		node = node->next;
	}
	return node;
}

HashNode* _new_hashnode(char* key, int val){
	HashNode* node = (HashNode*) malloc(sizeof(HashNode));
	node->key = malloc((strlen(key) + 1) * sizeof(char));
	strcpy(node->key, key);
	node->val = val;

	return node;
}


HashTable* create_table(int size){
	HashTable* table = (HashTable*) malloc(sizeof(HashTable));
	table->size = size;
	table->ocupation = 0;
	//NOTE: calloc seta os bytes para 0, 
	//porém nem todos sistemas NULL = 0
	table->elems = (HashNode**) calloc(size, sizeof(HashNode*)); 

	return table;
}

void _insert_node_list(HashTable* t, HashNode* node){
	if(node == NULL) return;
	_insert_node_list(t, node->next);
	node->next = NULL;
	insert_table(t, node->key, node->val);
}

HashTable* _rehash(HashTable* t){
	int new_size = t->size * 2 + 1;
	HashNode** resized_elems = (HashNode**) calloc(new_size, sizeof(HashNode*));
	
	HashNode** old_elems = t->elems;
	int old_size = t->size;
	
	t->elems = resized_elems;
	t->size = new_size;
	t->ocupation = 0;
	//insere os nós já alocados na lista
	for(int i = 0; i < old_size; i++)
		_insert_node_list(t, old_elems[i]);
	
	//só deleta a lista de pointers, pois os nodos estão sendo apontados agora pela nova lista
	free(old_elems);
	return t;
}


HashNode* insert_table(HashTable* t, char* key, int val){
	//if(t->ocupation++ > t->size){printf("rehash! o: %d, s: %d\n", t->ocupation, t->size); _rehash(t);}
	t->ocupation++;
	//se o nodo já exite só sobreescreve o valor
	HashNode* node = _find_node(t, key);
	if(node != NULL){
		node->val = val;
		return node;
	}
	node = _new_hashnode(key, val);
	int indice = _calc_hash(t, key);
	//coloca o nodo na frente da lista
	HashNode* node_at_adress = t->elems[indice];
	node->next = node_at_adress;
	t->elems[indice] = node;

	return node;
}

void _print_node_list(HashNode* node){
	if(node == NULL){
		printf("None;\n");
		return;
	}
	printf("(k: %s, v: %d) -> ", node->key, node->val);
	_print_node_list(node->next);
}

void print_table(HashTable* t){
	printf("Table size: %d, current ocupation: %d \n", t->size, t->ocupation);
	for(int i = 0; i < t->size; i++){
		if(t->elems[i] != NULL){
			printf("[%d] ", i);
			_print_node_list(t->elems[i]);
		}
	}
}
