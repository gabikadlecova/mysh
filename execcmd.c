#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include <error.h>
#include <errno.h>
#include "execcmd.h"
#include "common.h"
#include "commands.h"
#include "state.h"

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe);
int exec_pipe(struct cmdpipe *cp);

int exec_group(struct cmdgrp *cg) {
	int result = 0;

	struct pipeentry *p;
	STAILQ_FOREACH(p, &cg->subcmds, entries) {
		result = exec_pipe(p->p);
		if (result > SIG_VAL) {
			break;
		}
	}

	set_retval(result);
	return (result);
};

int exec_pipe(struct cmdpipe *cp) {
	int result = 0;
	int wstat;

	pid_t last_pid = -1;
	
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
			if (piperes == -1) {
				err(1, "cannot create pipe: ");
			}

			outfd = fd[1];
			prev_out = fd[0];
		}
	
		last_pid = exec_cmd(c->command, infd, outfd);
		ind++;
	}

	if (last_pid != 0) {
		waitpid(last_pid, &wstat, 0);

		if (WIFEXITED(wstat)) {
			result = WEXITSTATUS(wstat);
		}
		else if (WIFSIGNALED(wstat)) {
			int sig = WTERMSIG(wstat);
			fprintf(stderr, "Killed by signal %d\n", sig);
			result = sig + SIG_VAL;
		}
		else if (WIFSTOPPED(wstat)) {
			int sig = WSTOPSIG(wstat);
			fprintf(stderr, "Stopped by signal %d\n", WSTOPSIG(wstat));
			result = sig + SIG_VAL;
		}
		else {
			fprintf(stderr, "Child terminated with unknown status");
		}

	}
	else {
		result = get_retval(); // internal commands are expected to set retval
	}

	while (wait(NULL) != -1); // wait for the other pipe processes

	return (result);
};

char **args_to_list(struct cmd *c);

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe) {
	if (is_intern_cmd(c->path)) {
		
		char **argv = args_to_list(c);
		
		run_intern_cmd(c->path, c->argc + 1, argv); 

		free(argv);
		return (0);
	}

	
	pid_t pid = fork();
	ERR_EXIT(pid == -1);

	if (pid == 0) {
		// child
		if (inpipe != -1) {
			dup2(inpipe, 0);

			if (inpipe != 0) {
				close(inpipe);
			}
		}

		if (outpipe != -1) {
			dup2(outpipe, 1);

			if (outpipe != 1) {
				close(outpipe);
			}
		}
		
		// todo redirections

		char **args = args_to_list(c);
			
		execvp(c->path, args);
		
		err(127, c->path);

	}

	if (inpipe != -1) {
		close(inpipe);
	}

	if (outpipe != -1) {
		close(outpipe);
	}
	
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
