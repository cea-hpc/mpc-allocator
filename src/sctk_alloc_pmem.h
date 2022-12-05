#ifndef SCTK_ALLOC_PMEM_H
#define SCTK_ALLOC_PMEM_H

#define _GNU_SOURCE

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sctk_alloc_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SCTK_ROUNDUP(x,y) (ceil((float)x/y)*y)

#ifndef S_ISCHR
#define S_ISCHR(x) ((x & S_IFMT) == S_IFCHR)
#endif

typedef enum
{
	TYPE_ERROR = -1,
	TYPE_DEVDAX = 0,
	TYPE_REGULAR = 1
} SCTK_PMEM_FILE_TYPE;

SCTK_PUBLIC void* sctk_pmem_create(const char*, size_t*);
SCTK_PUBLIC int sctk_pmem_destroy(void*, size_t);
SCTK_INTERN SCTK_PMEM_FILE_TYPE get_file_type(const char*);
SCTK_INTERN int create_tmp_file(const char*);
SCTK_INTERN int declare_random_access(int, const size_t);
SCTK_INTERN int allocate_file_space(int fd, const size_t);

#ifdef __cplusplus
}
#endif

#endif //SCTK_ALLOC_PMEM_H
