#include "commands.h"
#include "common.h"
#include "string.h"

void add_arg(struct cmd *c, const char *argtext) {
	TRY_MALLOC(struct argentry, arg);
	arg->text = strdup(argtext); // argtext could be freed elsewhere

	if (STAILQ_EMPTY(c->argv)) {
		STAILQ_INSERT_HEAD(&c->argv, arg, entries);
	}
	else {
		STAILQ_INSERT_TAIL(&c->argv, arg, entries);
	}

	c->argc++;
};

void add_in(struct cmd *c, const char *intext) {
	c->inpath = strdup(intext);
	ERR_EXIT(c->inpath == NULL, EXIT_FAILURE, NULL);
};

void add_out(struct cmd *c, const char *outtext, bool isappend) {
	c->outpath = strdup(outtext);
	ERR_EXIT(c->outpath == NULL, EXIT_FAILURE, NULL);

	c->isappend = isappend;
};


void push_command(struct cmdpipe *cp, struct cmd *command) {
	TRY_MALLOC(struct cmdentry, new_cmd);
	new_cmd->command = command;
	
	STAILQ_INSERT_HEAD(&cp->commands, new_cmd, entries);
	
	cp->cmdc++;
};

void push_pipe(struct cmdgroup *cg, struct cmdpipe *cp) {
	TRY_MALLOC(struct pipeentry, new_pipe);
	new_pipe->p = cp;

	STAILQ_INSERT_HEAD(&cg->pipes, new_pipe, entries);

	cg->pipec++;
};

struct cmd *new_command() {
	TRY_MALLOC(struct cmd, new_cmd);

	new_cmd->path = NULL;
	
	STAILQ_INIT(&new_cmd->argv)
	
	new_cmd->inpath = NULL;
	new_cmd->outpath = NULL;
	new_cmd->isappend = false;

	return (new_cmd);
};

struct cmdpipe *new_pipeline() {
	TRY_MALLOC(struct cmdpipe, new_pipe);

	STAILQ_INIT(&new_pipe->commands);
	new_pipe->cmdc = 0;

	return (new_pipe);
};

struct cmdgroup *new_group() {
	TRY_MALLOC(struct cmdgroup, new_grp);

	STAILQ_INIT(&new_grp->pipes);
	new_grp->pipec = 0;

	return (new_grp);
};
