#ifndef _COMMON_H_
#define	_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#define	CREATE_DYN(TYPE) ( \
	TYPE *ptr; \
	if ((ptr = (TYPE *) malloc(sizeof (TYPE))) == NULL) { \
		fprintf(stderr, "malloc failed (%s)\n", strerror(errno)); \
		exit(1); \
	} \
)

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

#endif // _COMMON_H_
