#include "execcmd.h"
#include "common.h"
#include <unistd.h>

int exec_cmd(struct cmd *c);
int exec_pipe(struct cmdpipe *cp);

int exec_group(struct cmdgroup *cg) {
	int result;

	STAILQ_FOREACH(p, &cg->pipes, entries) {
		result = exec_pipe(p);
	}

	// wait should be here as not to hang

	return (result);
};

int exec_pipe(struct cmdpipe *cp) {
	// should be created from the end
};

char **args_to_list(struct cmd *c);

pid_t exec_cmd(struct cmd *c) {
	pid_t pid = fork();
	ERR_EXIT(pid == -1, EXIT_FAILURE, "cannot fork");

	if (pid > 0) {
		// child
		
		// todo redirections

		char **args = args_to_list(c);
		execvp(c->path, args);
	}
	
	return (pid);
};
