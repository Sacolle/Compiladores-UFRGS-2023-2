#
# UFRGS - Compiladores B - Marcelo Johann - 2023/2 - Etapa 3
#
# Nome: Pedro Henrique Boniatti Colle
# Matrícula: 00333916
#

# make run roda por default com o arquivo de entrada teste.txt
# para usar outro, rode make run in=NAME

ifndef $(in)
	in := teste.txt
endif

ifndef $(out)
	out := out.txt
endif


FLAGS = -std=c99 -pedantic -Wall -Werror

CC = clang


etapa3: lex.yy.o y.tab.o main.o hashtable.o ast.o
	$(CC) $(FLAGS) -o etapa3 lex.yy.o y.tab.o main.o hashtable.o ast.o
y.tab.c: parser.y ast.h hashtable.h
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
run: ./etapa3
	./etapa3 $(in) $(out)
clean:
	rm -f *.o lex.yy.c lex.yy.h y.tab.c y.tab.h etapa3 y.output
