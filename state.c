#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/queue.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "state.h"

/* List entry - internal command. */
SLIST_HEAD(intern_head, intern_cmd);
struct intern_cmd {
	SLIST_ENTRY(intern_cmd) entries;

	char *name;
	sh_func func;
};

/* Shell state. */
struct mysh_state {
	bool pwd_set;
	bool oldpwd_set;

	int retval;

	struct intern_head intern_cmds;
};

/* Default state. */
struct mysh_state state = {
	.pwd_set = false,
	.oldpwd_set = false,
	.retval = 0,
	.intern_cmds = SLIST_HEAD_INITIALIZER(.intern_cmds)
};


/*
 * Set environmental or internal variable.
 *
 * Only PWD and OLDPWD supported in this version.
 */
int set_var(char *varname, const char *value, bool is_exported) {
	if (strcmp(varname, "PWD") == 0) {
		if (value != NULL) {
			int res = setenv(varname, value, 1);
			if (res == -1) {
				err(EXIT_FAILURE,
					"cannot set environmental var: ");
			}

			// indicates that the value is valid
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
				err(EXIT_FAILURE,
					"cannot set environmental var: ");
			}

			// indicates that the value is valid
			state.oldpwd_set = true;

			return (res);
		}
		else {
			state.oldpwd_set = false;
		}
	}
	else {
		// not implemented

		if (is_exported) {
			// tbd in the future
		}

		errno = ENOSYS;
		return (-1);
	}

	return (0);
}

/*
 * Gets environmental or internal variable by name. Returns NULL
 * if the variable is not set.
 *
 * Only PWD and OLDPWD are supported in this version.
 */
char *get_var(char *varname) {
	if (strcmp(varname, "PWD") == 0) {
		errno = 0; // NULL is a valid return value
		return (state.pwd_set ? strdup(getenv("PWD")) : NULL);
	}
	else if (strcmp(varname, "OLDPWD") == 0) {
		errno = 0; // NULL is a valid return value
		return (state.oldpwd_set ? strdup(getenv("OLDPWD")) : NULL);
	}
	else {
		errno = ENOSYS;
		return (NULL);
	}

	return (0);
}

/* Sets last return value. */
void set_retval(int value) {
	state.retval = value;
}

/* Gets last return value. */
int get_retval() {
	return (state.retval);
}

/* Checks whether there exists a internal command with the specified name. */
bool is_intern_cmd(char *cmd) {
	struct intern_cmd *icmd;

	SLIST_FOREACH(icmd, &state.intern_cmds, entries) {
		if (strcmp(icmd->name, cmd) == 0) {
			return (true);
		}
	}

	return (false);
}

/*
 * Runs an internal cmd with specified arguments.
 *
 * Returns -1 if the command is not an internal command.
 */
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
}

/*
 * Registers an internal command in the shell state.
 */
void add_intern_cmd(char *cmd, sh_func cmd_func) {
	struct intern_cmd *icmd = malloc(sizeof (struct intern_cmd));
	ERR_EXIT(icmd == NULL);

	icmd->name = strdup(cmd);
	ERR_EXIT(icmd->name == NULL);

	icmd->func = cmd_func;

	SLIST_INSERT_HEAD(&state.intern_cmds, icmd, entries);
}

/* Resets the state to default (no internal commands, no variables,...) */
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
}
