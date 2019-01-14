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
	int opt;

	if (argc == 1) {
		ret_val = run_interactive();
		return (ret_val);
	}

	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
			case 'c':
				ret_val = run_string_cmd(optarg);
				return (ret_val);
			default:
				fprintf(stderr, "usage: mysh | mysh [-c Cmd] | mysh arg1");
				return (1);

		}
	}
	
	if (argc > 2) {
		fprintf(stderr, "usage: mysh | mysh [-c Cmd] | mysh arg1");
		return (1);
	}

	run_file(argv[optind]);
	return (ret_val);
}
