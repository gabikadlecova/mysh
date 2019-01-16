HEADERS = *.h myshlex.h mysh.tab.h

OBJ = ./out
BIN = ./bin

SRC_FILES = commands.c execcmd.c core.c state.c main.c
OBJ_FILES = $(SRC_FILES:%.c=$(OBJ)/%.o)

bison_fl = mysh.tab.h mysh.tab.c
flex_fl = lex.yy.c myshlex.h

CFLAGS = -Wall -Wextra -O0


.PHONY: all clean

all: $(BIN)/mysh

$(BIN)/mysh: $(OBJ_FILES) $(OBJ)/mysh.tab.o $(OBJ)/lex.yy.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) $(OBJ)/mysh.tab.o $(OBJ)/lex.yy.o -lreadline

$(OBJ)/mysh.tab.o: mysh.tab.c myshlex.h
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) mysh.tab.c -o $(OBJ)/mysh.tab.o

$(OBJ)/lex.yy.o: lex.yy.c mysh.tab.h
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) lex.yy.c -o $(OBJ)/lex.yy.o

mysh.tab.h: mysh.tab.c
mysh.tab.c: lex.yy.c mysh.y
	bison -d mysh.y

myshlex.h: lex.yy.c
lex.yy.c: mysh.l
	flex mysh.l

$(OBJ_FILES) : $(OBJ)/%.o : %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(BIN)/* $(OBJ)/* $(bison_fl) $(flex_fl)
	rmdir $(OBJ)
	rmdir $(BIN)
