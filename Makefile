CC=cc
DEPS = *.h
OBJ = commands.o execcmd.o

mysh: bison flex mysh.l mysh.y $(OBJ)
	$(CC) -o $@ $(OBJ) mysh.tab.c lex.yy.c

bison:
	bison -d mysh.y

flex:
	flex mysh.l

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

clean:
	rm lex.yy.c mysh.tab.c mysh $(OBJ)

