#include "btree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>

LtMap* ltmap_create(){
	LtMap* map = (LtMap*) malloc(sizeof(LtMap));
	map->root = NULL;
	return map;
}

LtMapNode* ltmap_node_make(char* key, int first_use, int last_use){
	LtMapNode* node = (LtMapNode*) malloc(sizeof(LtMapNode));
	node->key = key;
	node->first_use = first_use;
	node->last_use = last_use;
	node->left = NULL;
	node->right = NULL;
	return node;	
}

void ltmap_node_insert(LtMapNode* node, char* key, int first_use, int last_use){
	int cmp = strcmp(node->key, key);
	if(cmp < 0){
		if(node->left == NULL){
			node->left = ltmap_node_make(key, first_use, last_use);
			return;
		}
		ltmap_node_insert(node->left, key, first_use, last_use);
	}else if(cmp > 0){
		if(node->right == NULL){
			node->right = ltmap_node_make(key, first_use, last_use);
			return;
		}
		ltmap_node_insert(node->right, key, first_use, last_use);
	}else{ 
		fprintf(stderr, "Attemp to insert value that already exists.\nExitting...\n\n");
		exit(7);
		return;
	}
}

void ltmap_insert(LtMap* map, char* key, int first_use, int last_use){
	if(map->root == NULL){
		map->root = ltmap_node_make(key, first_use, last_use);
		return;
	}
	ltmap_node_insert(map->root, key, first_use, last_use);
}

LtMapNode* ltmap_node_get(LtMapNode* node, char* key){
	if(node == NULL) return NULL;

	int cmp = strcmp(node->key, key);
	if(cmp < 0){
		return ltmap_node_get(node->left, key);
	}else if(cmp > 0){
		return ltmap_node_get(node->right, key);
	}else{ 
		//cmp == 0
		return node;
	}
}

LtMapNode* ltmap_get(LtMap* map, char* key){
	if(map == NULL) return NULL;
	return ltmap_node_get(map->root, key);
}

void ltmap_node_print(LtMapNode* node, int depth){
	if(node == NULL) return;
	for(int i = 0; i < depth; i++) printf(" ");
	printf("%s: (%d, %d)\n", node->key, node->first_use, node->last_use);
	
	ltmap_node_print(node->left, depth + 1);
	ltmap_node_print(node->right, depth + 1);
}

void ltmap_print(LtMap* map){
	ltmap_node_print(map->root, 0);
}
void ltmap_node_free(LtMapNode* node){
	if(node == NULL) return;
	ltmap_node_free(node->left);
	ltmap_node_free(node->right);
	free(node);
}
void ltmap_free(LtMap* map){
	if(map == NULL) return;
	ltmap_node_free(map->root);
	free(map);
}





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
