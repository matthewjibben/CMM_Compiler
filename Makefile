cmm:  cmmGrammar.l sample.cmm
	flex cmmGrammar.l
	cc lex.yy.c -lfl
	./a.out < sample.cmm
