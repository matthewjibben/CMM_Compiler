cmm:  cmm.l cmm.y tokens.h symboltable.c sample.cmm parser.c ast.h ast.c
	bison -d cmm.y
	flex cmm.l
	cc -g -o cmm parser.c symboltable.c ast.c cmm.tab.c lex.yy.c -lm
	./cmm
	make clean

clean: 
	rm -f a.out
	rm -f lex.yy.c
	rm -f cmm.tab.c
	rm -f cmm.tab.h
	rm -f cmm
