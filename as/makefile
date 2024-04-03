CC = gcc
CFLAGS = -Wall -g -Iobj -Isrc
PARSER = parser
LEXER = lexer

all: bin/tpcas

bin/tpcas: obj/$(LEXER).o obj/$(PARSER).o obj/tree.o obj/table_symbol.o obj/nasm_translator.o obj/semantic_analyzer.o obj/parse_ast.o obj/utils.o
	$(CC) -o $@ $^

exe: _anonymous.o
	gcc -o exe _anonymous.o -nostartfiles -no-pie

_anonymous.o: _anonymous.asm
	nasm -f elf64 -o _anonymous.o _anonymous.asm

obj/parse_ast.o: src/parse_ast.c src/parse_ast.h

obj/semantic_analyzer.o: src/semantic_analyzer.c src/semantic_analyzer.h

obj/tree.o: src/tree.c src/tree.h

obj/nasm_translator.o: src/nasm_translator.c src/nasm_translator.h

obj/table_symbol.o: src/table_symbol.c src/table_symbol.h

obj/utils.o: src/utils.c src/utils.h

obj/$(PARSER).o: obj/$(PARSER).c src/tree.h

obj/$(LEXER).o: obj/$(LEXER).c obj/$(PARSER).h

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

obj/$(LEXER).c: src/$(LEXER).l obj/$(PARSER).h
	flex -o $@ $<

obj/$(PARSER).c obj/$(PARSER).h: src/$(PARSER).y
	bison -d -o obj/$(PARSER).c $<

.PHONY: clean

clean:
	rm -f obj/*.o obj/$(LEXER).c obj/$(PARSER).c obj/$(PARSER).h bin/tpcas