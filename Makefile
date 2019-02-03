cmm:  cmm.l sample.cmm
	flex cmm.l
	cc lex.yy.c -lfl
	./a.out < sample.cmm
	make clean

clean: 
	rm -f a.out
	rm -f lex.yy.c
