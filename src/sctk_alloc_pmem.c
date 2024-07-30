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

#include "sctk_alloc_pmem.h"

SCTK_PUBLIC sctk_pmem_desc_t* sctk_pmem_create(const char* dir, size_t* size)
{

  sctk_pmem_desc_t *cur_desc = NULL;
  cur_desc = (sctk_pmem_desc_t*) malloc(sizeof(sctk_pmem_desc_t));
  cur_desc->cur_size = 0x0;
  cur_desc->max_size = 0x0;
  cur_desc->ptr   = NULL;
  cur_desc->start = NULL;

  SCTK_PMEM_FILE_TYPE type = TYPE_ERROR;
  long int alignment = sysconf(_SC_PAGE_SIZE);

  /* Print Banner */
  fprintf(stdout, "System Page size: %ld\n", alignment);

  type = get_file_type(dir);

  switch(type)
  {
    case TYPE_REGULAR:
      fprintf(stdout, "File Type: Regular\n");
      break;
    case TYPE_DEVDAX:
      fprintf(stdout, "File Type: Device\n");
      break;
    case TYPE_ERROR:
    default:
      fprintf(stdout, "File Type: Unknown\n");
      return NULL;
  }

  *size = SCTK_ROUNDUP(*size, alignment);
  fprintf(stdout, "Requested size: %ld bytes\n", *size);

  if(type == TYPE_REGULAR)
  {
#ifdef O_TMPFILE

    int fd = create_tmp_file(dir);

    /* Declare random access pattern for fd */
    if(declare_random_access(fd, *size) != 0)
    {
      fprintf(stderr, "Error: fail of advise access pattern for pmem\n");
      return NULL;
    }

    /* Allocate the file space for fd */
    if(allocate_file_space(fd, *size) != 0)
    {
      fprintf(stderr, "Error: unable to ensure that pmem space has been allocated..\n");
      return NULL;
    }

    void* base = mmap(NULL, *size, PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE|MAP_SYNC, fd, 0);
    if(base == NULL)
    {
      fprintf(stderr, "Error: unable to map a file into %s\n", dir);
      return NULL;
    }

    close(fd);

    cur_desc->max_size = *size;
    cur_desc->ptr = base;
    cur_desc->start = base;

    return cur_desc;

#else
    fprintf(stderr, "O_TMPFILE is not defined on the system...\n");
    return NULL;
#endif
  }
  else if(type == TYPE_DEVDAX)
  {
    fprintf(stderr, "Error: not supported now...\n");
    free(cur_desc);
    return NULL;
  }

  free(cur_desc);
  return NULL;
}

SCTK_PUBLIC int sctk_pmem_destroy(sctk_pmem_desc_t* base, size_t size)
{
  int ret = munmap(base->start, size);
  base->ptr = NULL;
  free(base);
  return ret;
}

SCTK_INTERN SCTK_PMEM_FILE_TYPE get_file_type(const char* dir)
{
  fprintf(stdout, "Checking directory \"%s\"...\n", dir);

  /* Check the access to the file 'dev' */
  if(access(dir, F_OK) != 0)
  {
    fprintf(stderr, "Can't access to \"%s\"\n", dir);
    return -1;
  }

  struct stat st;
  /* Get Information about the file pointed by 'dev' */
  stat(dir, &st);

  /* Check if 'st' point to a character device (DEVDAX = char device) */
  if(S_ISCHR(st.st_mode))
  {
    /* Get the device ID */
    dev_t device_id = st.st_rdev;

    /* Get major (class of the device) and minor  */
    unsigned int major = major(device_id);
    unsigned int minor = minor(device_id);

    char path[8192];
    snprintf(path, 8192, "/sys/dev/char/%u:%u/subsystem", major, minor);

    fprintf(stdout, "Device Path: %s\n", path);

    char rpath[8192];
    char* tpath = realpath(path, rpath);
    if(tpath == NULL)
    {
      fprintf(stderr, "Unable to resolve device path...\n");
      return -1;
    }
    fprintf(stdout, "Real Path: %s\n", rpath);

    /*TODO: Is the real path containing "/dev/dax" string?
     * If yes: DEVDAX supported
     * If no: error
     * */

    return TYPE_DEVDAX;
  }
  else
  {
    /* Check if dev refers to a directory */
    if((st.st_mode & S_IFMT) != S_IFDIR)
    {
      fprintf(stderr, "Fatal Error: %s is not a directory\n", dir);
      return TYPE_ERROR;
    }

    return TYPE_REGULAR;
  }
}

SCTK_INTERN int create_tmp_file(const char* dir)
{
  /* Create a new temporary file (read/write permissions) into dir directory */
  //int fd = open(dir, O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR);
  int fd = open(dir, O_TMPFILE | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
  if(fd < 0)
  {
    fprintf(stderr, "Fatal Error: unable to open directory %s (Error code: %d)\n", dir, errno);
    return -1;
  }
  return fd;
}

/* Predeclaration of random access pattern for fd */
SCTK_INTERN int declare_random_access(int fd, const size_t size)
{
    return posix_fadvise(fd, 0, size, POSIX_FADV_RANDOM);
}

/* Try to allocate space into file fd to ensure that memory space is allocated */
SCTK_INTERN int allocate_file_space(int fd, const size_t size)
{
  return posix_fallocate(fd, 0, size);
}


SCTK_PUBLIC void* sctk_pmem_malloc(sctk_pmem_desc_t* desc, size_t size)
{
  void* ret = NULL;
  if(size + desc->cur_size <= desc->max_size)
  {
    desc->cur_size += size;
    ret = desc->ptr;
    desc->ptr += size;
  }
  return ret;
}

SCTK_PUBLIC void  sctk_pmem_free(sctk_pmem_desc_t* desc, size_t size, void* ptr)
{
  memset(ptr, 0x0, size);
}
