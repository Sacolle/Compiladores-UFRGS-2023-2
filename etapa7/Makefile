#
# UFRGS - Compiladores B - Marcelo Johann - 2023/2 - Etapa 6
#
# Nome: Pedro Henrique Boniatti Colle
# Matrícula: 00333916
#

# make run roda por default com o arquivo de entrada teste.txt
# para usar outro, rode make run in=NAME

ifndef $(in)
	in := teste.txt
endif

FLAGS = -std=c99 -pedantic -Wall -Werror -g
CC = clang

include depends.mk

#define DEPS = 
#endef

SRCFOLDER = src
OBJFOLDER = build/obj
PREFOLDER = build/pre

SRC = $(notdir $(wildcard src/*/*.c) $(wildcard $(PREFOLDER)/*.c)) main.c

OBJS = $(addprefix $(OBJFOLDER)/, $(patsubst %.c, %.o, $(SRC)))

build/etapa7: $(OBJS)
	$(CC) $(FLAGS) -o build/etapa7 $(OBJS) 

$(PREFOLDER)/y.tab.h $(PREFOLDER)/y.tab.c: $(parser)
	yacc -d -v $< -b build/pre/y

$(PREFOLDER)/lex.yy.h $(PREFOLDER)/lex.yy.c: $(scanner)
	flex -o build/pre/lex.yy.c --header-file=build/pre/lex.yy.h $<  

# mágica
# https://www.gnu.org/software/make/manual/html_node/Static-Usage.html
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html


.SECONDEXPANSION:
$(OBJS): $(OBJFOLDER)/%.o: $$($(strip %))
	$(CC) $(FLAGS) -c $< -o $@ 

#declara as palavras reservadas (comandos)
.PHONY: clean run echo depends test

depends:
	node scripts/buildDeps.js

test: ./build/etapa7
	./tests/tests.sh

echo:
	echo $(OBJS)

run: ./build/etapa7
	./build/etapa7 $(in)

clean:
	find ~/compiladores/etapa7/build -type f -delete

#gcc -m32 out.s -o out -g