%{
# include "commands.h"
# include "execcmd.h"
# include <stdio.h>
# include <stdlib.h>

# include <string.h>

extern int yylineno;

%}

%union {
	char *str;
	struct cmd *command;
	struct cmdgrp *group;
	struct cmdpipe *pipeline;
}

%token <str> TEXT
%token NL
%token SEMICOLON
%token PIPE
%token REDIR_R
%token REDIR_A
%token REDIR_L

%type <pipeline> pipeln
%type <group> grp
%type <command> comtokens com

%define parse.error verbose


%%

cmdline
   :
   | cmdline NL
   | cmdline grp NL { printf("1"); exec_group($2); free_group($2); }
   ;

grp
  : pipeln { printf("2"); $$ = new_group(); push_pipe($$, $1); }
  | pipeln SEMICOLON { $$ = new_group(); push_pipe($$, $1); }
  | pipeln SEMICOLON grp { $$ = $3; push_pipe($$, $1); }
  ;

pipeln
  : com { printf("3"); $$ = new_pipeline(); push_command($$, $1); }
  | com PIPE pipeln { $$ = $3; push_command($$, $1); }
  ;

com
  : comtokens
  | com REDIR_R TEXT { $$ = $1; add_out($$, $3, false); }
  | com REDIR_L TEXT { $$ = $1; add_in($$, $3); }
  | com REDIR_A TEXT { $$ = $1; add_out($$, $3, true); }
  ;

comtokens
  : TEXT { printf("4 value: %s", $1); $$ = new_command(); $$->path = strdup($1);  }
  | comtokens TEXT { printf("5 value: %s", $1); $$ = $1; add_arg($$, strdup($2)); }
  ;

%%

int yyerror(char *s) {
	fprintf(stderr, "error: %s\n", s);
};

