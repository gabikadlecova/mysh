#ifndef _STATE_H_
#define _STATE_H_

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

int set_var(char *varname, char *value, bool is_exported);
char *get_var(char *varname, bool is_exported);

void set_retval(int value);
int get_retval();

pid_t get_sh_pgid();
void set_sh_pgid(pid_t pgid);

bool is_intern_cmd(char *cmd);
int run_intern_cmd(char *cmd, char **argv);

typedef int (*sh_func)(char **argv);
int add_intern_cmd(char *cmd, sh_func cmd_func);

#endif // _STATE_H_
