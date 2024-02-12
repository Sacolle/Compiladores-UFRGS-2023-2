ast = src/data_structures/ast.c src/data_structures/ast.h src/data_structures/hashtable.h
btree = src/data_structures/btree.c src/data_structures/btree.h
hashtable = src/data_structures/hashtable.c src/data_structures/hashtable.h
tac = src/data_structures/tac.c src/data_structures/tac.h src/data_structures/hashtable.h src/data_structures/ast.h
main = src/main.c build/pre/lex.yy.h src/parsers/tokens.h src/parsers/semantics.h src/parsers/gen_asm.h src/data_structures/hashtable.h src/data_structures/ast.h src/data_structures/tac.h
gen_asm = src/parsers/gen_asm.c src/parsers/gen_asm.h src/data_structures/hashtable.h src/data_structures/btree.h src/parsers/semantics.h src/data_structures/tac.h
parser = src/parsers/parser.y src/data_structures/hashtable.h src/data_structures/ast.h
scanner = src/parsers/scanner.l src/data_structures/hashtable.h src/data_structures/ast.h build/pre/y.tab.h
semantics = src/parsers/semantics.c src/parsers/semantics.h src/data_structures/ast.h src/data_structures/hashtable.h
lex.yy = build/pre/lex.yy.c
y.tab = build/pre/y.tab.c