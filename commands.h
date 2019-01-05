#ifndef _COMMANDS_H_
#define	_COMMANDS_H_

#include <stdio.h>
#include <stdbool.h>
#include <queue.h>

STAILQ_HEAD(arghead, argentry);
struct argentry {
	STAILQ_ENTRY(argentry) entries;
	char *text;
};

struct cmd {
	char *path;
	struct arghead argv;
	int argc;

	char *inpath;
	char *outpath;
	bool isappend;
};

STAILQ_HEAD(cmdhead, cmdentry);

struct cmdentry {
	STAILQ_ENTRY(cmdentry) entries;
	struct cmd *command;
};

struct cmdpipe {
	struct cmdhead commands;
	int cmdc;
};

STAILQ_HEAD(grouphead, pipeentry);

struct pipeentry {
	STAILQ_ENTRY(pipeentry) entries;
	struct cmdpipe *p;
};

struct cmdgroup {
	struct pipehead pipes;
	int pipec;
};

void add_arg(struct cmd *c, const char *argtext);
void add_in(struct cmd *c, const char *intext);
void add_out(struct cmd *c, const char *outtext, bool isappend);

void push_command(struct cmdpipe *cp, struct cmd *command);

void push_pipe(struct cmdgroup *cg, struct cmdpipe *cp);

struct cmd *new_command();
struct cmdpipe *new_pipeline();
struct cmdgroup *new_group();

void free_group(struct cmdgroup *cg);
#endif // _COMMANDS_H_
