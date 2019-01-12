%option noyywrap
%option header-file="myshlex.h"

%{
# include "mysh.tab.h"
%}

%%

[a-zA-Z0-9/_.-]+ { yylval.str = yytext; return TEXT; } 
";"             { return SEMICOLON; }
"|"             { return PIPE; }
">"             { return REDIR_R; }
">>"            { return REDIR_A; }
"<"             { return REDIR_L; }
"\n"            { return NL; }
"#"[^\n]*       {}
.               {}

%%
