#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "core.h"


int
main(int argc, char **argv)
{
	int ret_val = 0;
	char *usage = "usage: mysh | mysh [-c Cmd] | mysh arg1";

	// no arguments - interactive mode
	if (argc == 1) {
		ret_val = run_interactive();
		return (ret_val);
	}

	int opt;
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
			// string command mode
			case 'c':
				ret_val = run_string_cmd(optarg);
				return (ret_val);
			// unsupported
			default:
				fprintf(stderr, usage);
				return (1);

		}
	}

	// too many arguments
	if (argc > 2) {
		fprintf(stderr, usage);
		return (1);
	}

	// single string argument - string command mode
	ret_val = run_file(argv[optind]);
	return (ret_val);
}
