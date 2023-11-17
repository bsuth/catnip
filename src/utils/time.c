#include "time.h"

uint32_t
get_time_msec()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return 1000 * now.tv_sec + now.tv_nsec / 1000000;
}
