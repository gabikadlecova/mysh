#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "core.h"


int main(int argc, char **argv)
{
	int ret_val = run_interactive();

	return (ret_val);
}
