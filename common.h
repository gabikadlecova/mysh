#ifndef _COMMON_H_
#define	_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#define	ERR_RETURN(cond, ret_val) \
	if (cond) { \
		return (ret_val); \
	} \

#define	ERR_ERRNO_RETURN(cond, exit_val, ret_val) \
	if (cond) { \
		errno = exit_val; \
		return (ret_val); \
	} \

#define	ERR_EXIT(cond) \
	if (cond) { \
		err(EXIT_FAILURE, strerror(errno)); \
	} \

#endif // _COMMON_H_
