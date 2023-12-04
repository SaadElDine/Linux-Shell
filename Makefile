#Use GNU compiler
cc = gcc -g
CC = g++ -g
#Not more than variables
#kelmet all is the target
#all keyword 
all: shell

lex.yy.o: shell.l 
	lex shell.l  
	$(cc) -c lex.yy.c
#In summary, running lex shell.l processes the Lex source file shell.l and generates
#a C source file (lex.yy.c) that implements the lexer based on the lexical rules
#and patterns defined in shell.l. This generated lexer can be compiled and linked
#with other code to create a complete program for tokenizing and processing input text
#according to the specified rules.
y.tab.o: shell.y
	yacc -d shell.y
	$(CC) -c y.tab.c

command.o: command.cc
	$(CC) -c command.cc
#compile command.cc to command.o

shell: y.tab.o lex.yy.o command.o
	$(CC) -o shell lex.yy.o y.tab.o command.o -ll
#shell fel awal bt crate el files deh 
#-ll links lex library 
#-o shell is the output file
#Type gcc c –o [program_name].exe [program_name]. c
#lex.yy.o y.tab.o command.o are the input files
clean:
	rm -f lex.yy.c y.tab.c  y.tab.h shell *.o

