all:
	lex pcl.lex
	bison -d pcl.y
	g++ -ggdb -std=c++17  lex.yy.c pcl.tab.c -o test.out -lboost_program_options

draw: all
	./test.out < example.pcl > out.dot
	dot -Tpng out.dot -o graph.png