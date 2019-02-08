/*
 * Starts the shell in one of the three modes.
 * 	1. Interactive (reads from standard input)
 *	2. File mode (opens and reads a file with commands)
 *	3. String mode (executes a command string)
 */

#ifndef _CORE_H_
#define	_CORE_H_

int run_interactive();
int run_file(char *file_name);
int run_string_cmd(char *cmds);

#endif // _CORE_H_
