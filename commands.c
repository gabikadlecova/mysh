#include <stdlib.h>
#include "commands.h"
#include "common.h"
#include "string.h"

/*
 * Copies the path string to the command structure.
 */
void add_path(struct cmd *c, const char *path) {
	c->path = strdup(path);
	ERR_EXIT(c->path == NULL);
}

/*
 * Creates an arg queue entry, copies the argument text and
 * adds it to the arg list.
 */
void add_arg(struct cmd *c, const char *argtext) {
	struct argentry * arg = malloc(sizeof (struct argentry));
	ERR_EXIT(arg == NULL);

	arg->text = strdup(argtext);
	ERR_EXIT(arg->text == NULL);

	if (STAILQ_EMPTY(&c->argv)) {
		STAILQ_INSERT_HEAD(&c->argv, arg, entries);
	}
	else {
		STAILQ_INSERT_TAIL(&c->argv, arg, entries);
	}

	c->argc++;
}

/*
 * Copies input file path to the command structure.
 */
void add_in(struct cmd *c, const char *intext) {
	c->inpath = strdup(intext);
	ERR_EXIT(c->inpath == NULL);
}

/*
 * Copies output file path to the command structure. Enables to set
 * the append flag.
 */
void add_out(struct cmd *c, const char *outtext, bool isappend) {
	c->outpath = strdup(outtext);
	ERR_EXIT(c->outpath == NULL);

	c->isappend = isappend;
}

/*
 * Pushes a command to the head of a command pipeline.
 */
void push_command(struct cmdpipe *cp, struct cmd *command) {
	struct cmdentry *new_cmd = malloc(sizeof (struct cmdentry));
	ERR_EXIT(new_cmd == NULL);

	new_cmd->command = command;

	STAILQ_INSERT_HEAD(&cp->commands, new_cmd, entries);

	cp->cmdc++;
}

/*
 * Pushes a command pipeline to the head of a command group.
 */
void push_pipe(struct cmdgrp *cg, struct cmdpipe *cp) {
	struct pipeentry *new_pipe = malloc(sizeof (struct pipeentry));
	ERR_EXIT(new_pipe == NULL);

	new_pipe->p = cp;

	STAILQ_INSERT_HEAD(&cg->subcmds, new_pipe, entries);

	cg->pipec++;
}

/*
 * Creates a new command with default member values.
 */
struct cmd *new_command() {
	struct cmd *new_cmd = malloc(sizeof (struct cmd));
	ERR_EXIT(new_cmd == NULL);

	new_cmd->path = NULL;

	STAILQ_INIT(&new_cmd->argv);
	new_cmd->argc = 0;

	new_cmd->inpath = NULL;
	new_cmd->outpath = NULL;
	new_cmd->isappend = false;

	return (new_cmd);
}

/*
 * Creates a new command pipeline with default member values.
 */
struct cmdpipe *new_pipeline() {
	struct cmdpipe *new_pipe = malloc(sizeof (struct cmdpipe));
	ERR_EXIT(new_pipe == NULL);

	STAILQ_INIT(&new_pipe->commands);
	new_pipe->cmdc = 0;

	return (new_pipe);
}

/*
 * Creates a new command group with default member values.
 */
struct cmdgrp *new_group() {
	struct cmdgrp *new_grp = malloc(sizeof (struct cmdgrp));
	ERR_EXIT(new_grp == NULL);

	STAILQ_INIT(&new_grp->subcmds);
	new_grp->pipec = 0;

	return (new_grp);
}

/*
 * Frees the command including its members.
 */
void free_command(struct cmd *c) {
	free(c->path);
	c->path = NULL;

	while (!STAILQ_EMPTY(&c->argv)) {
		struct argentry *a = STAILQ_FIRST(&c->argv);
		STAILQ_REMOVE_HEAD(&c->argv, entries);

		free(a->text);
		free(a);

		c->argc--;
	}

	free(c->inpath);
	c->inpath = NULL;

	free(c->outpath);
	c->outpath = NULL;

	c->isappend = false;
	c->argc = 0;

	free(c);
}

/*
 * Frees the command pipeline along with its members.
 */
void free_pipe(struct cmdpipe *cp) {
	while (!STAILQ_EMPTY(&cp->commands)) {
		struct cmdentry *comm = STAILQ_FIRST(&cp->commands);
		STAILQ_REMOVE_HEAD(&cp->commands, entries);

		free_command(comm->command);
		free(comm);

		cp->cmdc--;
	}

	free(cp);
}

/*
 * Frees the command group along with its members.
 */
void free_group(struct cmdgrp *cg) {
	while (!STAILQ_EMPTY(&cg->subcmds)) {
		struct pipeentry *p = STAILQ_FIRST(&cg->subcmds);
		STAILQ_REMOVE_HEAD(&cg->subcmds, entries);

		free_pipe(p->p);
		free(p);

		cg->pipec--;
	}

	free(cg);
}
