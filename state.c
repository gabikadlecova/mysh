#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/queue.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "state.h"

SLIST_HEAD(intern_head, intern_cmd);
struct intern_cmd {
	SLIST_ENTRY(intern_cmd) entries;
	
	char *name;
	sh_func func;
};


struct mysh_state {
	bool pwd_set;
	bool oldpwd_set;
	
	int retval;

	struct intern_head intern_cmds;
};

struct mysh_state state = {
	.pwd_set = false,
	.oldpwd_set = false,
	.retval = 0,
	.intern_cmds = SLIST_HEAD_INITIALIZER(.intern_cmds)
};



int set_var(char *varname, const char *value, bool is_exported) {
	if (strcmp(varname, "PWD") == 0) {
		if (value != NULL) {
			int res = setenv(varname, value, 1);
			if (res == -1) {
				err(EXIT_FAILURE, "cannot set environmental var: ");
			}

			state.pwd_set = true;

			return (res);
		}
		else {
			state.pwd_set = false;
		}

	}
	else if (strcmp(varname, "OLDPWD") == 0) {
		if (value != NULL) {
			int res = setenv(varname, value, 1);
			if (res == -1) {
				err(EXIT_FAILURE, "cannot set environmental var: ");
			}

			state.oldpwd_set = true;

			return (res);
		}
		else {
			state.oldpwd_set = false;
		}
	}
	else {
		errno = ENOSYS;
		return (-1);
	}

	return (0);
};

char *get_var(char *varname) {
	if (strcmp(varname, "PWD") == 0) {
		return (state.pwd_set ? strdup(getenv("PWD")) : NULL);
	}
	else if (strcmp(varname, "OLDPWD") == 0) {
		return (state.oldpwd_set ? strdup(getenv("OLDPWD")) : NULL);
	}
	else {
		errno = ENOSYS;
		return (NULL);
	}

	return (0);
};

void set_retval(int value) {
	state.retval = value;
};

int get_retval() {
	return (state.retval);
};

bool is_intern_cmd(char *cmd) {
	struct intern_cmd *icmd;
	SLIST_FOREACH(icmd, &state.intern_cmds, entries) {
		if (strcmp(icmd->name, cmd) == 0) {
			return (true);
		}
	}

	return (false);
};

int run_intern_cmd(char *cmd, int argc, char **argv) {
	struct intern_cmd *icmd = SLIST_FIRST(&state.intern_cmds);
	while (icmd != NULL) {
		if (strcmp(icmd->name, cmd) == 0) {
			int res = icmd->func(argc, argv);
			set_retval(res);

			return (res);
		}

		icmd = SLIST_NEXT(icmd, entries);
	}
	
	return (-1);
};

void add_intern_cmd(char *cmd, sh_func cmd_func) {
	struct intern_cmd *icmd = (struct intern_cmd *) malloc(sizeof (struct intern_cmd));
	ERR_EXIT(icmd == NULL);

	icmd->name = strdup(cmd);
	ERR_EXIT(icmd->name == NULL);
	
	icmd->func = cmd_func;

	SLIST_INSERT_HEAD(&state.intern_cmds, icmd, entries);
};

void reset_state() {
	struct intern_cmd *icmd;

	while (!SLIST_EMPTY(&state.intern_cmds)) {
		icmd = SLIST_FIRST(&state.intern_cmds);
		SLIST_REMOVE_HEAD(&state.intern_cmds, entries);

		free(icmd->name);
		free(icmd);
	}

	state.retval = 0;
	state.pwd_set = false;
	state.oldpwd_set = false;
};
