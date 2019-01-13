#ifndef _STATE_H_
#define	_STATE_H_

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

int set_var(char *varname, const char *value, bool is_exported);
char *get_var(char *varname);

void set_retval(int value);
int get_retval();

bool is_intern_cmd(char *cmd);
int run_intern_cmd(char *cmd, int argc, char **argv);

typedef int (*sh_func)(int argc, char **argv);
void add_intern_cmd(char *cmd, sh_func cmd_func);

void reset_state();

#endif // _STATE_H_
