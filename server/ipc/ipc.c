#include "ipc.h"
#include "common/shared_memory.h"
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void
init_ipc()
{
  int shared_memory_fd =
    shm_open(CATNIP_SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0777);

  if (shared_memory_fd == -1) {
    g_warning("%s\n", get_shm_open_error_message());
    return;
  }

  ftruncate(shared_memory_fd, sizeof(struct catnip_shared_memory));

  struct catnip_shared_memory* catnip_shared_memory = mmap(
    NULL,
    sizeof(struct catnip_shared_memory),
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    shared_memory_fd,
    0
  );

  if (catnip_shared_memory == MAP_FAILED) {
    g_warning("%s\n", get_mmap_error_message());
    return;
  }

  memset(catnip_shared_memory, 0, sizeof(struct catnip_shared_memory));

  sprintf(catnip_shared_memory->test, "bye world\n");
}
