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
#include "myshlex.h"

int cd_internal(int argc, char **argv);

int init_pwd();
int init() {
	/* pwd setup */
	int pwd_res = init_pwd();
	// todo check retval
	
	/* pgid setup */
	int pgid_res = setpgid(0, 0); 
	ERR_EXIT(pgid_res == -1);

	/* internal commands */
	add_intern_cmd("cd", cd_internal);

	// set_retval(0); // init success
};

int init_pwd() {
	char *buf = (char *) malloc(sizeof(char) * PATH_MAX);
	ERR_EXIT(buf == NULL);

	char *res = getcwd(buf, PATH_MAX);
	if (res == NULL) {
		free(buf);
		ERR_EXIT(1);
	};

	int var_res = set_var("PWD", res, true);
	// todo errval
	var_res = set_var("OLD_PWD", NULL, true);
	
	free(res);
	return (0);
};

char *get_prompt() {
	char *pwd = get_var("PWD");
	char *pname = "mysh:";
	
	char *prompt = (char *) malloc((strlen(pwd) + strlen(pname) + 3));
	strcpy(prompt, pname);
	strcat(prompt, pwd);
	strcat(prompt, "> ");

	free(pwd);

	return (prompt);
};

bool rl_processing;
void sigint_handler_ia(int signo) {
	rl_crlf();
	if (rl_processing) {
		rl_on_new_line_with_prompt();
	}
	else {
		rl_on_new_line();
	}
	rl_replace_line("", 0);

	rl_redisplay();
};

int set_sigaction() {
	struct sigaction sa;
	sa.sa_handler = sigint_handler_ia;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	int sa_res = sigaction(SIGINT, &sa, NULL);
	// todo errors

	return (sa_res);
};

int parse_string(char *cmd_string);

int run_interactive() {
	init(); // todo errors
	
	rl_processing = false;

	int sa_res = set_sigaction();
	// todo errors
	
	char *buffer = NULL;
	char *prompt = get_prompt();
	while ((buffer = readline(prompt)) != NULL) {
		rl_processing = true;
		
		if (strlen(buffer) > 0) {
			add_history(buffer);
		}

		int parse_ret = parse_string(buffer);
		free(buffer);

		free(prompt);
		prompt = get_prompt();

		rl_processing = false;
	}

	free(prompt);
	write(1, "\n", 1);
	
	// todo return sth
	// todo return value handling etc
	
	// todo remove 
	reset_state();
	return (0);	
};

int run_file(FILE *fd) {
	return (0);
};

int run_string_cmd(char *cmds) {
	init(); // todo errorval

	int parse_ret = parse_string(cmds);
	// todo return sth, handle retval...

	// todo remove
	reset_state();
	return (0);
};

int parse_string(char *cmd_string) {
	int len;
	if ((len = strlen(cmd_string)) == 0) {
		return (0);
	}

	char *buffer = malloc(len + 2);
	ERR_EXIT(buffer == NULL);

	strcpy(buffer, cmd_string);
	strcat(buffer, "\n");

	YY_BUFFER_STATE buffer_state = yy_scan_string(buffer);
	int parse_ret = yyparse();
	yy_delete_buffer(buffer_state);

	free(buffer);
	return (parse_ret);
};

int cd_internal(int argc, char **argv) {
	char *dir = NULL;
	switch (argc) {
		case 1:
			dir = strdup(getenv("HOME"));
			ERR_EXIT(dir == NULL);
			break;
		case 2:
			if (argv[1] == "-") {
				dir = get_var("OLDPWD");
			}
			else {
				dir = strdup(argv[1]);
				ERR_EXIT(dir == NULL);
			}

			break;
		default:
			// todo fprintf
			return (-1);
	}

	if (dir == NULL) {
		return (-1);
	}

	

	// todo check errors
	int dir_res = chdir(dir);

	char *pwd = get_var("PWD");
	set_var("OLDPWD", pwd, true);
	
	set_var("PWD", dir, true);

	free(pwd);
	free(dir);

	// todo errors
	return (0);
};

