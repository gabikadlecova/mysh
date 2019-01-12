#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "myshlex.h"

int main(int argc, char **argv)
{
	char *buffer = NULL;
	size_t len = 0;
	ssize_t nread;
	char *prompt = "mysh> ";

	while ((buffer = readline(prompt)) != NULL) {
		if (strlen(buffer) > 0) {
			add_history(buffer);
		}

		char *ln = malloc(strlen(buffer) + 2);
		strcpy(ln, buffer);
		strcat(ln, "\n");

		YY_BUFFER_STATE buffer_state = yy_scan_string(ln);
		yyparse();
		yy_delete_buffer(buffer_state);		
		
		free(ln);
		free(buffer);
	}

	return (0);
}
