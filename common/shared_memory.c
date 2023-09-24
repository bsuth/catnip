#include "shared_memory.h"
#include <errno.h>
#include <unistd.h>

const char* CATNIP_SHARED_MEMORY_NAME = "/catnip";

const char*
get_shm_open_error_message()
{
  if (errno == EACCES) {
    return "cannot access";
  }

  return NULL;
}

const char*
get_mmap_error_message()
{
  if (errno == EACCES) {
    return "cannot access";
  }

  return NULL;
}
