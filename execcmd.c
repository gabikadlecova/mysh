#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include "execcmd.h"
#include "common.h"
#include "commands.h"

#include <errno.h>

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe);
pid_t exec_pipe(struct cmdpipe *cp);

int exec_group(struct cmdgrp *cg) {
	int result = 0;

	pid_t last_pid = -1;

	struct pipeentry *p;
	STAILQ_FOREACH(p, &cg->subcmds, entries) {
		last_pid = exec_pipe(p->p);
	}

	// todo improve, options, retvalue
	waitpid(last_pid, &result, 0);

	while (wait(NULL) != -1);

	
	return (result);
};

int exec_pipe(struct cmdpipe *cp) {
	pid_t last_pid;
	
	int prev_out = -1;
	int fd[2];
	int ind = 0;

	struct cmdentry *c;
	STAILQ_FOREACH(c, &cp->commands, entries) {
		int infd = -1;
		int outfd = -1;

		infd = prev_out;
		
		if (ind != (cp->cmdc - 1)) {
			int piperes = pipe(fd);
			// todo handle

			// todo closes??
			outfd = fd[1];
			prev_out = fd[0];
		}
	
		last_pid = exec_cmd(c->command, infd, outfd);
		ind++; // error-safe?
	}

	return (last_pid);
};

char **args_to_list(struct cmd *c);

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe) {
	pid_t pid = fork();
	ERR_EXIT(pid == -1);
	// todo handle better fork errno
	// todo ! signal handler change

	if (pid == 0) {
		// child
		if (inpipe != -1) {
			dup2(inpipe, 0);
			//todo handle both

			if (inpipe != 0) {
				close(inpipe);
			}
		}

		if (outpipe != -1) {
			dup2(outpipe, 1);
			//todo handle both

			if (outpipe != 1) {
				close(outpipe);
			}
		}
		
		// todo error handling in pipes		
		// todo redirections

		char **args = args_to_list(c);
			
		execvp(c->path, args);
		//todo handle
	}

	if (inpipe != -1) {
		close(inpipe);
	}

	if (outpipe != -1) {
		close(outpipe);
	}
	// todo possible frees? and closes +handle closes
	return (pid);
};

char *get_command_name(char *path) {
	if (path == NULL) {
		return (NULL);
	}

	size_t len = strlen(path);
	size_t last_sep = len;

	for (size_t p_ind = 0; p_ind < len; p_ind++) {
		if (path[p_ind] == '/') {
			last_sep = p_ind;
		}
	}

	if (last_sep == len) {
		char *res = strdup(path);
		ERR_EXIT(res == NULL);
		
		return (res);
	}

	char *res = (char *) malloc(sizeof(char) * (len - last_sep));
	ERR_EXIT(res == NULL);

	strcpy(res, &path[last_sep + 1]);

	return (res);
};

char **args_to_list(struct cmd *c) {
	char **args = (char **) malloc((c->argc + 2) * sizeof(char *));
	ERR_EXIT(args == NULL);
	
	int argind = 0;
	args[argind++] = get_command_name(c->path);
	
	struct argentry *a;
	STAILQ_FOREACH(a, &c->argv, entries) {
		char *dup = strdup(a->text);
		ERR_EXIT(dup == NULL);

		args[argind++] = dup;
	}

	args[argind] = (char *) NULL;

	return (args);
};
