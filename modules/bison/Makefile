all:
	lex pcl.lex
	bison -d pcl.y
	g++ -ggdb -std=c++17 lex.yy.c pcl.tab.c -o test.out