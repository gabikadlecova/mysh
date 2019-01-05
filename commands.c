#include "commands.h"
#include "common.h"
#include "string.h"

void add_arg(struct cmd *c, const char *argtext) {
	struct argentry *arg = CREATE_DYN(struct argentry);
	arg->text = strdup(argtex); // argtext could be freed elsewhere

	if (STAILQ_EMPTY(c->argv)) {
		STAILQ_INSERT_HEAD(&c->argv, arg, entries);
	}
	else {
		STAILQ_INSERT_TAIL(&c->argv, arg, entries);
	}

	c->argc++;
};

void add_in(struct cmd *c, const char *intext) {
	c->inpath = strdup(argtex);
	ERR_EXIT(c->inpath == NULL, EXIT_FAILURE, NULL);
};
