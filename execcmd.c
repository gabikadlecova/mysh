#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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

/* Executes a group of commands. */
int exec_group(struct cmdgrp *cg) {
	int result = 0;

	struct pipeentry *p;
	STAILQ_FOREACH(p, &cg->subcmds, entries) {
		result = exec_pipe(p->p);
		
		// pipeline terminated by a signal
		if (result > SIG_VAL) {
			// command group must be terminated as well
			break;
		}
	}

	// return value of the last command
	set_retval(result);
	return (result);
}

/* Executes a command pipeline. */
int exec_pipe(struct cmdpipe *cp) {
	int result = 0;
	pid_t last_pid = -1;

	// vars for pipe descriptor manipulation
	int prev_out = -1;
	int fd[2];
	int ind = 0;

	// connect and execute all commands
	struct cmdentry *c;
	STAILQ_FOREACH(c, &cp->commands, entries) {
		// io file descriptors
		int infd = -1;
		int outfd = -1;

		// input of the previous command (or -1)
		infd = prev_out;

		// open output file descriptor (unless it's the last command)
		if (ind != (cp->cmdc - 1)) {
			int piperes = pipe(fd);
			if (piperes == -1) {
				err(1, "cannot create pipe: ");
			}

			// writing
			outfd = fd[1];
			// reading
			prev_out = fd[0];
		}

		last_pid = exec_cmd(c->command, infd, outfd);
		ind++;
	}

	// check exit status of the last command
	int wstat;
	if (last_pid != 0) {
		waitpid(last_pid, &wstat, 0);

		if (WIFEXITED(wstat)) {
			// exited normally
			result = WEXITSTATUS(wstat);
		}
		else if (WIFSIGNALED(wstat)) {
			int sig = WTERMSIG(wstat);
			fprintf(stderr, "Killed by signal %d\n", sig);

			result = sig + SIG_VAL;
		}
		else if (WIFSTOPPED(wstat)) {
			int sig = WSTOPSIG(wstat);
			fprintf(stderr, "Stopped by signal %d\n",
				WSTOPSIG(wstat));

			result = sig + SIG_VAL;
		}
		else {
			fprintf(stderr,
				"Child terminated with unknown status\n");
		}

	}
	else {
		// internal commands should set retval
		result = get_retval();
	}

	// wait for the other pipe processes
	while (wait(NULL) != -1);

	return (result);
}

char **args_to_list(struct cmd *c);

/* Executes a command. */
pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe) {
	
	// separate handling of internal commands
	if (is_intern_cmd(c->path)) {
		char **argv = args_to_list(c);
		
		// argc is number of commands, not counting cmd path
		run_intern_cmd(c->path, c->argc + 1, argv);

		free(argv);
		return (0);
	}

	pid_t pid = fork();
	ERR_EXIT(pid == -1);

	if (pid == 0) {
		// child
		if (inpipe != -1) {
			// connect pipe input
			ERR_EXIT(dup2(inpipe, 0) == -1);

			if (inpipe != 0) {
				close(inpipe);
			}
		}

		if (outpipe != -1) {
			// connect pipe output
			ERR_EXIT(dup2(outpipe, 1) == -1);

			if (outpipe != 1) {
				close(outpipe);
			}
		}

		// redirection - read
		if (c->inpath != NULL) {
			int indes = open(c->inpath, O_RDONLY);
			if (indes == -1) {
				fprintf(stderr, "%s: %s\n", c->path,
					strerror(errno));

				return (1);
			}

			ERR_EXIT(dup2(indes, 0) == -1);

			if (indes != 0) {
				close(indes);
			}
		}

		// redirection - write (or append)
		if (c->outpath != NULL) {
			int flags;
			if (c->isappend) {
				flags = O_WRONLY | O_CREAT | O_APPEND;
			}
			else {
				flags = O_WRONLY | O_CREAT | O_TRUNC;
			}

			int outdes = open(c->outpath, flags, 0666);
			if (outdes == -1) {
				fprintf(stderr, "%s: %s\n", c->path,
					strerror(errno));

				return (1);
			}

			ERR_EXIT(dup2(outdes, 1) == -1)

			if (outdes != 1) {
				close(outdes);
			}
		}

		char **args = args_to_list(c);

		execvp(c->path, args);
		
		// invalid execution
		err(127, c->path);

	}

	// close descriptors in parent
	if (inpipe != -1) {
		close(inpipe);
	}

	if (outpipe != -1) {
		close(outpipe);
	}

	return (pid);
}

/* Gets command name without path for argv[0]. */
char *get_command_name(char *path) {
	if (path == NULL) {
		return (NULL);
	}

	size_t len = strlen(path);
	size_t last_sep = len;

	// find position of the last '/'
	for (size_t p_ind = 0; p_ind < len; p_ind++) {
		if (path[p_ind] == '/') {
			last_sep = p_ind;
		}
	}

	// possibly invalid command path
	if (last_sep == len) {
		char *res = strdup(path);
		ERR_EXIT(res == NULL);

		return (res);
	}

	// copy substring
	char *res = malloc(sizeof (char) * (len - last_sep));
	ERR_EXIT(res == NULL);

	strcpy(res, &path[last_sep + 1]);

	return (res);
}

/* Convert argument queue to argument array. */
char **args_to_list(struct cmd *c) {
	char **args = malloc((c->argc + 2) * sizeof (char *));
	ERR_EXIT(args == NULL);

	int argind = 0;
	// first argument is the command name by convention
	args[argind++] = get_command_name(c->path);

	// duplicate argument strings
	struct argentry *a;
	STAILQ_FOREACH(a, &c->argv, entries) {
		char *dup = strdup(a->text);
		ERR_EXIT(dup == NULL);

		args[argind++] = dup;
	}

	// terminating NULL (convention)
	args[argind] = (char *) NULL;

	return (args);
}
