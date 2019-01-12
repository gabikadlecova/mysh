%option noyywrap
%{
# include <stdio.h>
# include "mysh.tab.h"
%}

%%

[a-zA-Z0-9/_-]+ { yylval.str = yytext; return TEXT; } 
";"             { return SEMICOLON; }
"|"             { return PIPE; }
">"             { return REDIR_R; }
">>"            { return REDIR_A; }
"<"             { return REDIR_L; }
"\n"            { return NL; }
"#"[^\n]*       {}
.               {}

%%

int main(int argc, char **argv)
{
	char *buffer = NULL;
	ssize_t l = 0;

	while (1) {
		getline(&buffer, &l, stdin);

		YY_BUFFER_STATE buffer_state = yy_scan_string(buffer);
		yyparse();
		yy_delete_buffer(buffer_state);		
	}

	return (0);
}
