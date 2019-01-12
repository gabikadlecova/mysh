#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include "execcmd.h"
#include "common.h"
#include "commands.h"
#include <err.h>
#include <errno.h>

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe);
int exec_pipe(struct cmdpipe *cp);

int exec_group(struct cmdgrp *cg) {
	int result = 0;

	struct pipeentry *p;
	STAILQ_FOREACH(p, &cg->subcmds, entries) {
		exec_pipe(p->p);
	}

	// todo improve
	pid_t a = wait(NULL);
	// while (wait(result) != -1);
	printf("waited");
	printf("result: %d\n", a);
	return (result);
};

int exec_pipe(struct cmdpipe *cp) {
	int result;
	
	int *prev_fd = NULL;
	int ind = 0;

	struct cmdentry *c;
	STAILQ_FOREACH(c, &cp->commands, entries) {
		printf("command: %s", c->command->path);
		int infd = -1;
		int outfd = -1;

		/*if (prev_fd != NULL) {
			infd = prev_fd[1];
		}
		
		if (ind != (cp->cmdc - 1)) {
			printf("a jejda");
			int *fd = malloc(sizeof(int) * 2);
			pipe(fd);
			//todo handle errors

			outfd = fd[0];

			free(prev_fd);
			prev_fd = fd;
		}*/

		exec_cmd(c->command, infd, outfd);
	}

	free(prev_fd);

	return (0);
};

char **args_to_list(struct cmd *c);

pid_t exec_cmd(struct cmd *c, int inpipe, int outpipe) {
	pid_t pid = fork();
	printf("PID: %d\n", pid);
	ERR_EXIT(pid == -1, EXIT_FAILURE, "cannot fork");
	// todo handle better fork errno

	if (pid == 0) {
		// child
		/*if (inpipe != -1) {
			printf("prin");
			dup2(inpipe, 0);

			if (inpipe != 0) {
				close(inpipe);
			}
		}

		if (outpipe != -1) {
			printf("pr");
			dup2(outpipe, 1);
			
			if (outpipe != 1) {
				close(outpipe);
			}
		}*/
		
		// todo error handling in pipes		
		// todo redirections

		char **args = args_to_list(c);
		printf("%s\n", args[0]);
	
			
		int a = execvp(c->path, args);
		printf("%d\n", a);
		printf("errno %s\n", strerror(errno));
	}
	
	printf("ok");
	// todo possible frees?
	return (pid);
};

char *get_command_name(char *path) {
	// todo check path null
	size_t len = strlen(path);
	size_t last_sep = len;

	for (size_t p_ind = 0; p_ind < len; p_ind++) {
		if (path[p_ind] == '/') {
			last_sep = p_ind;
		}
	}

	if (last_sep == len) {
		return (strdup(path));
	}

	if (last_sep == (len - 1)) {
		// todo better
		return (NULL);
	}

	char *res = (char *) malloc(sizeof(char) * (len - last_sep));
	ERR_EXIT(res == NULL, EXIT_FAILURE, MALLOC_FAILED);

	strcpy(res, &path[last_sep + 1]);

	return (res);
};

char **args_to_list(struct cmd *c) {
	char **args = (char **) malloc((c->argc + 2) * sizeof(char *));
	ERR_EXIT(args == NULL, EXIT_FAILURE, MALLOC_FAILED);
	
	int argind = 0;

	// todo check null
	args[argind++] = get_command_name(c->path);
	
	struct argentry *a;
	STAILQ_FOREACH(a, &c->argv, entries) {
		args[argind++] = a->text;

	}

	printf("argc: %d\n", c->argc);
	printf("argind: %d\n", argind);
	args[argind] = (char *) NULL;

	return (args);
};
