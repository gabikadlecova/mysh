#ifndef _CORE_H_
#define _CORE_H_

int init();

int run_interactive();
int run_file(FILE *fd);
int run_string_cmd(char *cmds);

int exit();

#endif // _CORE_H_
