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
#include "sctk_alloc_debug.h"
#include "sctk_alloc_posix.h"
#include "sctk_alloc_topology.h"
#include "sctk_alloc_chain.h"
#include "sctk_alloc_light_mm_source.h"

#include "sctk_alloc_on_gpu.h"
#include "cuda.h"

/************************* GLOBALS *************************/
/*#ifdef HAVE_HWLOC
/**
 * We create an allocation chain per node to use sctk_malloc_on_node. At init
 * we plug them on the related NUMA memory source.
 * Those chains are shared between all threads.
**/
/*static struct sctk_alloc_chain sctk_global_alloc_on_node_chain[SCTK_MAX_NUMA_NODE];
/**
 * By default we prefer to use separeted memory source which force the binding, but for future maybe
 * we can provide some options to use the common once from allocator, but they are less safe as
 * they didn't force the NUMA bindind.
**/
/*static struct sctk_alloc_mm_source_light sctk_global_alloc_on_node_mm_src[SCTK_MAX_NUMA_NODE];
/**

 * For debug, to check if it was initilized before usage.
**/
/*static bool sctk_global_alloc_on_node_initilized = false;
#endif //HAVE_HWLOC*/

/********************* CHECK CU ERRORS *********************/
// This will output the proper error strings
// in the event that a CUDA driver call returns an error
#define CHECK_ERROR(f) {\
	CUresult err = f; \
	if (err != CUDA_SUCCESS) {\
        fprintf(stderr, "\nERROR : cuda driver API call FAILED !\n\tfile : %s\n\tline : %d\n",__FILE__, __LINE__); \
    	exit (EXIT_FAILURE);}}



/************************* FUNCTION ************************/
/**
 * Request memory allocation on a GPU, without specification
 * @param size Size of the memory bloc to allocate.
 */
SCTK_PUBLIC void * sctk_malloc_on_gpu (size_t size)
{
    // init
    cuInit(0);

    // get first CUDA device
    CUdevice device;
    CHECK_ERROR(cuDeviceGet(&device, 0););

    // create context
    CUcontext context;
    CHECK_ERROR(cuCtxCreate(&context, 0, device));

    // allocate
    CUdeviceptr ptr;
    CHECK_ERROR(cuMemAlloc(&ptr, size));

    // return the pointer
    return (void*)ptr;
}
