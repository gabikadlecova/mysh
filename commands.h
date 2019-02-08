/*
 * This header contains declarations of structures
 * representing a command line structure.
 *
 * Functions for manipulating the structure (creation,
 * destruction) are provided as well.
 */

#ifndef _COMMANDS_H_
#define	_COMMANDS_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/queue.h>

/*
 * Queue entry - argument of a command.
 */
STAILQ_HEAD(arghead, argentry);
struct argentry {
	STAILQ_ENTRY(argentry) entries;
	char *text;
};

/*
 * Single command with arguments, input file name (or NULL if not
 * provided), output file name (or NULL if not provided).
 *
 * If there is an output file name, isappend indicates whether
 * it should be opened for append or truncated.
 *
 */
struct cmd {
	char *path;
	struct arghead argv;
	int argc;

	char *inpath;
	char *outpath;
	bool isappend;
};

/*
 * Queue entry - command.
 */
STAILQ_HEAD(cmdhead, cmdentry);
struct cmdentry {
	STAILQ_ENTRY(cmdentry) entries;
	struct cmd *command;
};

/*
 * Commands connected via pipes.
 */
struct cmdpipe {
	struct cmdhead commands;
	int cmdc;
};

/*
 * Queue entry - pipe.
 */
STAILQ_HEAD(pipehead, pipeentry);
struct pipeentry {
	STAILQ_ENTRY(pipeentry) entries;
	struct cmdpipe *p;
};

/*
 * A group of sequential commands or command pipelines.
 */
struct cmdgrp {
	struct pipehead subcmds;
	int pipec;
};

/* Command data setters. */
void add_path(struct cmd *c, const char *path);
void add_arg(struct cmd *c, const char *argtext);
void add_in(struct cmd *c, const char *intext);
void add_out(struct cmd *c, const char *outtext, bool isappend);

/* Adds a command to the head of the pipeline. */
void push_command(struct cmdpipe *cp, struct cmd *command);

/* Adds a command pipeline to the head of the command group. */
void push_pipe(struct cmdgrp *cg, struct cmdpipe *cp);

/* Construct command line structures. */
struct cmd *new_command();
struct cmdpipe *new_pipeline();
struct cmdgrp *new_group();

/* Free the command line structures. */
void free_command(struct cmd *c);
void free_pipe(struct cmdpipe *cp);
void free_group(struct cmdgrp *cg);

#endif // _COMMANDS_H_
