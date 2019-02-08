/*
 * Provides functions for executing the command line.
 */

#ifndef _EXECCMD_H_
#define	_EXECCMD_H_

#include "commands.h"

/* Executes a group of commands. */
int exec_group(struct cmdgrp *cg);

#endif // _EXECCMD_H_
