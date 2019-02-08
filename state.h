/*
 * Represents the state of the shell. Provides functions
 * for variable manipulation, internal commands and return
 * value of the last command.
 */

#ifndef _STATE_H_
#define	_STATE_H_

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

/*
 * Enables to set an internal/environmental variable.
 *
 * Is limited only to PWD and OLDPWD environmental variables
 * and does not support internal variables (would be added
 * in future versions).
 */
int set_var(char *varname, const char *value, bool is_exported);
char *get_var(char *varname);

/* Stores/gets return value of the last command. */
void set_retval(int value);
int get_retval();

/* Functions for internal command manipulation. */
bool is_intern_cmd(char *cmd);
int run_intern_cmd(char *cmd, int argc, char **argv);

typedef int (*sh_func)(int argc, char **argv);
void add_intern_cmd(char *cmd, sh_func cmd_func);

/* Resets the whole state including internal commands and variables. */
void reset_state();

#endif // _STATE_H_
