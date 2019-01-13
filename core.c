#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "common.h"
#include "core.h"
#include "state.h"

int setup_cwd();
int init() {
	/* pwd setup */
	int cwd_res = init_cwd();
	// todo check retval
	
	/* pgid setup */
	pid_t sh_pid = getpid():

	/* according to the manpage, this call should be safe,
	   as we use the pid of the calling process */
	int pgid_res = setpgid(sh_pid, 0); 
	ERR_EXIT(pgid_res == -1);

	set_sh_pgid(sh_pid);

	set_retval(0); // init success
};

int init_cwd() {
	char *buf = (char *) malloc(sizeof(char) * PATH_MAX);
	ERR_EXIT(buf == NULL);

	char *res = getcwd(buf, PATH_MAX);
	ERR_EXIT(res == NULL); // todo better exit (return val...)

	int var_res = set_var("PWD", res, true);
	// todo errval

	return (0);
};

int sigint_handler_ia(int signo) {

};

int set_sigaction() {
	struct sigaction sa, old_sa;
	sa.sa_handler = sigint_handler_ia;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	int sa_res = sigaction(SIGINT, &sa, &old_sa);
	// todo errors

	return (sa_res);
};

int parse_string(char *cmd_string);

int run_interactive() {
	
	int sa_res = set_sigaction();
	// todo errors
	
	char *buffer = NULL;
	char *prompt = get_prompt();
	while ((buffer = readline(prompt)) != NULL) {
		if (strlen(buffer) > 0) {
			add_history(buffer);
		}

		int parse_ret = parse_string(buffer);
		free(buffer);

		free(prompt);
		prompt = get_prompt();
	}

	free(prompt);
	
	// todo return value handling etc	
};

int run_file(FILE *fd) {

};

int run_string_cmd(char *cmds) {
	return (parse_string(cmds));
};

int parse_string(char *cmd_string) {
	int len;
	if ((len = strlen(cmds)) == 0) {
		return (0);
	}

	char *buffer = malloc(len + 2);
	ERR_EXIT(buffer == NULL);

	strcpy(buffer, cmds);
	strcat(buffer, "\n");

	YY_BUFFER_STATE buffer_state = yy_scan_string(buffer);
	int parse_ret = yyparse();
	yy_delete_buffer(buffer_state);

	free(buffer);
	return (parse_ret);
};
