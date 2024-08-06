#ifndef CATNIP_STATE_KEYBOARDS_H
#define CATNIP_STATE_KEYBOARDS_H

#include "objects/vector.h"
#include <wayland-server-core.h>

struct catnip_keyboards_state {
  catnip_vector_t* keyboards;

  struct {
    struct wl_signal create;
  } signals;
};

extern struct catnip_keyboards_state catnip_keyboards_state;

void
catnip_keyboards_init();

#endif
