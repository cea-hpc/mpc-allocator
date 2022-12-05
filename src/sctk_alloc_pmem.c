#include "sctk_alloc_pmem.h"

SCTK_PUBLIC void* sctk_pmem_create(const char* dir, size_t* size)
{
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
    return base;

#else
    fprintf(stderr, "O_TMPFILE is not defined on the system...\n");
    return -1;
#endif
  }
  else if(type == TYPE_DEVDAX)
  {
    fprintf(stderr, "Error: not supported now...\n");
    return NULL;
  }

  return NULL;
}

SCTK_PUBLIC int sctk_pmem_destroy(void* base, size_t size)
{
  return munmap(base, size);
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
