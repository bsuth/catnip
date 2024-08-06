#ifndef CATNIP_OBJECTS_SIGNAL_H
#define CATNIP_OBJECTS_SIGNAL_H

#include <stddef.h>

typedef struct _catnip_signal catnip_signal_t;

typedef void (*catnip_signal_callback)(void* invariant, void* data);

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_signal_t*
catnip_signal_create();

void
catnip_signal_destroy(catnip_signal_t* signal);

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_signal_subscribe(
  catnip_signal_t* signal,
  catnip_signal_callback callback,
  void* invariant
);

void
catnip_signal_unsubscribe(
  catnip_signal_t* signal,
  catnip_signal_callback callback
);

void
catnip_signal_publish(struct _catnip_signal* signal, void* data);

#endif
