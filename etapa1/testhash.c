/*
* Nome: Pedro Henrique Boniatti Colle
* Matrícula: 00333916
*/

#include<stdio.h>

#include "hashtable.h"

int main(){
	HashTable* table = create_table(4);
	insert_table(table, "Hello", 1);
	insert_table(table, "World", 2);
	insert_table(table, "!", 3);
	insert_table(table, "Nice to meet you", 4);
	insert_table(table, "goomba", 5);
	insert_table(table, "mario", 6);
	insert_table(table, "goomba", 7);
	print_table(table);
}