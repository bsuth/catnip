#include "../common/shared_memory.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

static struct catnip_shared_memory* catnip_shared_memory;

void
init_catnip_shared_memory()
{
  int shared_memory_fd = shm_open(CATNIP_SHARED_MEMORY_NAME, O_RDWR, 0777);

  if (shared_memory_fd == -1) {
    printf("%s\n", get_shm_open_error_message());
    exit(errno);
  }

  catnip_shared_memory = mmap(
    NULL,
    sizeof(struct catnip_shared_memory),
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    shared_memory_fd,
    0
  );

  if (catnip_shared_memory == MAP_FAILED) {
    printf("%s\n", get_mmap_error_message());
    exit(errno);
  }
}

int
main(int argc, char* argv[])
{
  init_catnip_shared_memory();

  printf("%s\n", catnip_shared_memory->test);

  return 0;
}
