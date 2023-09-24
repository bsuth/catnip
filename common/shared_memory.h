#ifndef CATNIP_COMMON_SHARED_MEMORY_H
#define CATNIP_COMMON_SHARED_MEMORY_H

extern const char* CATNIP_SHARED_MEMORY_NAME;

struct catnip_shared_memory {
  char test[256];
};

const char*
get_shm_open_error_message();

const char*
get_mmap_error_message();

#endif
