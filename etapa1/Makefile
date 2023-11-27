#
# UFRGS - Compiladores B - Marcelo Johann - 2023/2 - Etapa 1
#
# Nome: Pedro Henrique Boniatti Colle
# Matrícula: 00333916
#

etapa1: lex.yy.c main.c hashtable.c
	gcc -o etapa1 lex.yy.c main.c hashtable.c
lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l


clean:
	rm lex.yy.c etapa1
