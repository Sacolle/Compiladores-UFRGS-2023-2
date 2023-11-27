#
# UFRGS - Compiladores B - Marcelo Johann - 2023/2 - Etapa 1
#
# Nome: Pedro Henrique Boniatti Colle
# Matrícula: 00333916
#

# make run roda por default com o arquivo de entrada teste.txt
# para usar outro, rode make run in=NAME

ifndef $(in)
	in := teste.txt
endif


FLAGS = -std=c99 -pedantic -Wall -Werror=implicit-function-declaration

CC = clang


etapa2: lex.yy.o y.tab.o main.o hashtable.o
	$(CC) $(FLAGS) -o etapa2 lex.yy.o y.tab.o main.o hashtable.o
y.tab.c: parser.y
	yacc -d -v parser.y
lex.yy.c: scanner.l y.tab.c
	flex --header-file=lex.yy.h scanner.l
# mágica
# https://www.gnu.org/software/make/manual/html_node/Static-Usage.html
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
%.o: %.c	
	$(CC) $(FLAGS) -c $< -o $@ 

#declara as palavras reservadas (comandos)
.PHONY: clean run
run: ./etapa2
	./etapa2 $(in)
clean:
	rm -f *.o lex.yy.c lex.yy.h y.tab.c y.tab.h etapa2 y.output
