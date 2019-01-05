%{
# include "commands.h"
# include "execcmd.h"

extern int yylineno;

%}

%union {
	char *arg;
	struct cmd *command;
	struct cmdgroup *group;
	struct cmdpipe *pipeline;
}

%token <arg> TEXT
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
   | cmdline grp NL { exec_group($2); free_group($2); }
   ;

grp
  : pipeln { $$ = new_group(); push_pipe($$, $1); }
  | pipeln SEMICOLON { $$ = new_group(); push_pipe($$, $1); }
  | pipeln SEMICOLON grp { $$ = $3; push_pipe($$, $1); }
  ;

pipeln
  : com { $$ = new_pipeline(); push_command($$, $1); }
  | com PIPE pipeln { $$ = $3; push_command($$, $1); }
  ;

com
  : comtokens
  | com REDIR_R TEXT { $$ = $1; add_out($$, $3, false); }
  | com REDIR_L TEXT { $$ = $1; add_in($$, $3); }
  | com REDIR_A TEXT { $$ = $1; add_out($$, $3, true); }
  ;

comtokens
  : TEXT { $$ = new_command(); $$->path = $1;  }
  | comtokens TEXT { $$ = $1; add_arg($$, $2); }
  ;

%%
