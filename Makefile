CC=cc
DEPS = *.h
OBJ = commands.o execcmd.o core.o state.o main.o
CFLAGS = -g

mysh: bison flex mysh.l mysh.y $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) mysh.tab.c lex.yy.c -lreadline

bison:
	bison -d mysh.y

flex:
	flex mysh.l

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm lex.yy.c mysh.tab.h mysh.tab.c myshlex.h mysh $(OBJ)

