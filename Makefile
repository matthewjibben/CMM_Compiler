cmm:  cmm.l tokens.h symboltable.h symboltable.c sample.cmm
	flex cmm.l
	cc lex.yy.c symboltable.c -lfl
	./a.out < sample.cmm
	make clean

clean: 
	rm -f a.out
	rm -f lex.yy.c
