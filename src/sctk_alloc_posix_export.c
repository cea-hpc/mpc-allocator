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
/* #   - Valat Sébastien sebastien.valat@cea.fr                           # */
/* #                                                                      # */
/* ######################################################################## */


/************************** HEADERS ************************/
#include <stdlib.h>
#include <errno.h>
#include "sctk_alloc_inlined.h"
#include "sctk_alloc_posix.h"
#include "sctk_alloc_debug.h"

/************************* FUNCTION ************************/
void * calloc (size_t nmemb, size_t size)
{
	void * res;

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();
	res = sctk_calloc(nmemb,size);
	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();
	SCTK_ALLOC_MMCHECK_REG(res,nmemb*size,true);

	return res;
}

/************************* FUNCTION ************************/
void * malloc (size_t size)
{
	void * res;

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();
	res = sctk_malloc(size);
	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();
	SCTK_ALLOC_MMCHECK_REG(res,size,false);

	return res;
}

/************************* FUNCTION ************************/
void free (void * ptr)
{

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();
	SCTK_ALLOC_MMCHECK_UNREG(ptr);
	sctk_free(ptr);
	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();

}

/************************* FUNCTION ************************/
/** @todo do some check at compile time, because, some OS have more parameters in cfree prototype. **/
void cfree(void * ptr)
{

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();
	SCTK_ALLOC_MMCHECK_UNREG(ptr);
	sctk_free(ptr);
	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();

}

/************************* FUNCTION ************************/
void * realloc (void * ptr, size_t size)
{
	void * res;

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();
	SCTK_ALLOC_MMCHECK_UNREG(ptr);
	res = sctk_realloc(ptr,size);
	/**
	 * Improve the zeroed mapping here, we need to check the cases depending on new alloc or resize.
	**/
	SCTK_ALLOC_MMCHECK_REG(res,size,true);
	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();

	return res;
}

/************************* FUNCTION ************************/
int posix_memalign(void **memptr, size_t boundary, size_t size)
{
	int res = 0;



	//limit imposed by posix_memalign linux manpage
	if (boundary % sizeof(void*) != 0 && sctk_alloc_is_power_of_two(boundary))
	{
		sctk_alloc_pwarning("memalign boundary not power of 2 or boundary not multiple of sizeof(void*).");
		res = EINVAL;
	} else {
		*memptr = sctk_memalign(boundary,size);
	}


	return res;
}

/************************* FUNCTION ************************/
void * memalign(size_t boundary,size_t size)
{
	void * res;

	SCTK_ALLOC_MMCHECK_DISABLE_REPORT();

	//limit imposed by posix_memalign linux manpage
	if (sctk_alloc_is_power_of_two(boundary))
	{
		res = sctk_memalign(boundary,size);
	} else {
		sctk_alloc_pwarning("memalign boundary not power of 2.");
		res = NULL;
	}

	SCTK_ALLOC_MMCHECK_ENABLE_REPORT();
	SCTK_ALLOC_MMCHECK_REG(res,size,false);


	return res;
};

/************************* FUNCTION ************************/
void *valloc(size_t size)
{
	return memalign(SCTK_ALLOC_PAGE_SIZE,size);
}

