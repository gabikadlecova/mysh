#ifndef _COMMON_H_
#define	_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#define	ERR_CHECK(cond, err_no, ret_val) ( \
	if (cond) { \
		errno = err_no; \
		return (ret_val); \
	} \
)

#define	ERR_EXIT(cond, exit_val, err_text) ( \
	if (cond) { \
		err(exit_val, err_text) \
	} \
)

#define	TRY_MALLOC(type, var_name) ( \
	type *var_name = (type *) malloc(sizeof (type)); \
	ERR_EXIT(var_name == NULL, EXIT_FAILURE, "malloc failed"); \
)

#endif // _COMMON_H_
