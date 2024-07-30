/* ############################# MPC License ############################## */
/* # Wed Nov 19 15:19:19 CET 2008                                         # */
/* # Copyright or (C) or Copr. Commissariat a l'Energie Atomique          # */
/* #                                                                      # */
/* # IDDN.FR.001.230040.000.S.P.2007.000.10000                            # */
/* # This file is part of the MPC Runtime.                                # */
/* #                                                                      # */
/* # This software is governed by the CeCILL-C license under French law   # */
/* # and abiding by the rules of distribution of free software.  You can  # */
/* # use, modify and/ or redistribute the software under the terms of     # */
/* # the CeCILL-C license as circulated by CEA, CNRS and INRIA at the     # */
/* # following URL http://www.cecill.info.                                # */
/* #                                                                      # */
/* # The fact that you are presently reading this means that you have     # */
/* # had knowledge of the CeCILL-C license and that you accept its        # */
/* # terms.                                                               # */
/* #                                                                      # */
/* # Authors:                                                             # */
/* #   - Roussel Adrien adrien.roussel@cea.fr                             # */
/* #                                                                      # */
/* ######################################################################## */

#ifndef SCTK_ALLOC_PMEM_H
#define SCTK_ALLOC_PMEM_H

#define _GNU_SOURCE

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>
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

typedef struct
{
  uint64_t cur_size;
  uint64_t max_size;
  void* ptr;
  void* start;
} sctk_pmem_desc_t;

SCTK_PUBLIC sctk_pmem_desc_t* sctk_pmem_create(const char*, size_t*);
SCTK_PUBLIC int sctk_pmem_destroy(sctk_pmem_desc_t*, size_t);
SCTK_PUBLIC void* sctk_pmem_malloc(sctk_pmem_desc_t*, size_t);
SCTK_PUBLIC void  sctk_pmem_free(sctk_pmem_desc_t*, size_t, void*);
SCTK_INTERN SCTK_PMEM_FILE_TYPE get_file_type(const char*);
SCTK_INTERN int create_tmp_file(const char*);
SCTK_INTERN int declare_random_access(int, const size_t);
SCTK_INTERN int allocate_file_space(int fd, const size_t);

#ifdef __cplusplus
}
#endif

#endif //SCTK_ALLOC_PMEM_H
