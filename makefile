CC = gcc
CFLAGS = -Wall -g -Iobj -Isrc
PARSER = parser
LEXER = lexer

bin/tpcc: obj/$(LEXER).o obj/$(PARSER).o obj/tree.o obj/table_symbol.o obj/parse_ast.o obj/error.o obj/semantic_analyzer.o obj/code_generator.o
	$(CC) -o $@ $^

exe: _anonymous.o
	gcc -o exe _anonymous.o -nostartfiles -no-pie
	rm _anonymous.o

compile:
	./bin/tpcc  < ./gct.tpc

compile_tab:
	./bin/tpcc -s < ./gct.tpc

_anonymous.o: compile _anonymous.asm
	nasm -f elf64 -o _anonymous.o _anonymous.asm

obj/parse_ast.o: src/parse_ast.c src/parse_ast.h

obj/error.o: src/error.c src/error.h

obj/semantic_analyzer.o: src/semantic_analyzer.c src/semantic_analyzer.h

obj/code_generator.o: src/code_generator.c src/code_generator.h

obj/table_symbol.o: src/table_symbol.c src/table_symbol.h

obj/tree.o: src/tree.c src/tree.h

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
	rm -f obj/*.o obj/$(LEXER).c obj/$(PARSER).c obj/$(PARSER).h bin/tpcc exe _anonymous.asm