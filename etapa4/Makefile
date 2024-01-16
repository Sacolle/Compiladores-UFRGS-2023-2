#
# UFRGS - Compiladores B - Marcelo Johann - 2023/2 - Etapa 4
#
# Nome: Pedro Henrique Boniatti Colle
# Matrícula: 00333916
#

# make run roda por default com o arquivo de entrada teste.txt
# para usar outro, rode make run in=NAME

ifndef $(in)
	in := teste.txt
endif

FLAGS = -std=c99 -pedantic -Wall -Werror
CC = clang

SRC = lex.yy.c y.tab.c main.c hashtable.c ast.c semantics.c

OBJS = $(patsubst %.c, %.o, $(SRC))

etapa4: $(OBJS)
	$(CC) $(FLAGS) -o etapa4 $(OBJS) 
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
.PHONY: clean run echo

echo:
	echo $(OBJS)

run: ./etapa4
	./etapa4 $(in)
clean:
	rm -f *.o lex.yy.c lex.yy.h y.tab.c y.tab.h etapa4 y.output
