all:
	lex pcl.lex
	bison -d pcl.y
	g++ -ggdb lex.yy.c pcl.tab.c -o test.out