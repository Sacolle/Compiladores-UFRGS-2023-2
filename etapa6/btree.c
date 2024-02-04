#include "btree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Btree* btree_make_node(char* key, char* val){
	Btree* node = (Btree*) malloc(sizeof(Btree));
	node->key = key;
	node->val = val;
	node->left = NULL;
	node->right = NULL;

	return node;	
}

Btree* btree_create(char* key, char* val){
	return btree_make_node(key, val);
}

void btree_insert(Btree* tree, char* key, char* val){
	int cmp = strcmp(tree->key, key);

	if(cmp < 0){
		if(tree->left == NULL){
			tree->left = btree_make_node(key, val);
			return;
		}
		btree_insert(tree->left, key, val);
	}else if(cmp > 0){
		if(tree->right == NULL){
			tree->right = btree_make_node(key, val);
			return;
		}
		btree_insert(tree->right, key, val);
	}else{ 
		//cmp == 0
		//replace the value if it exists
		tree->val = val;
		return;
	}
}

char* btree_get(Btree* tree, char* key){
	if(tree == NULL) return NULL;

	int cmp = strcmp(tree->key, key);
	if(cmp < 0){
		return btree_get(tree->left, key);
	}else if(cmp > 0){
		return btree_get(tree->right, key);
	}else{ 
		//cmp == 0
		return tree->val;
	}
}

char* try_btree_get(Btree* tree, char* key){
	if(tree == NULL) return key;

	int cmp = strcmp(tree->key, key);
	if(cmp < 0){
		return btree_get(tree->left, key);
	}else if(cmp > 0){
		return btree_get(tree->right, key);
	}else{ 
		//cmp == 0
		return tree->val;
	}
}

int mymax(int a, int b){
	return a > b ? a : b;
}

int btree_height(Btree* tree){
	if(tree == NULL) return 0;
	return mymax(btree_height(tree->left) + 1, btree_height(tree->right) + 1);
}

void _btree_print(Btree* tree, int depth){
	if(tree == NULL) return;
	for(int i = 0; i < depth; i++) printf(" ");
	printf("%s: %s\n", tree->key, tree->val);
	
	_btree_print(tree->left, depth + 1);
	_btree_print(tree->right, depth + 1);
}

void btree_print(Btree* tree){
	_btree_print(tree, 0);
}


void btree_free(Btree* tree){
	if(tree == NULL) return;
	btree_free(tree->left);
	btree_free(tree->right);
	free(tree);
}
