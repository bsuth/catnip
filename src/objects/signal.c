#include "signal.h"
#include "objects/vector.h"
#include <stdlib.h>

struct _catnip_signal {
  catnip_vector_t* subscriptions;
};

struct _catnip_signal_subscription {
  catnip_signal_callback callback;
  void* invariant;
};

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static struct _catnip_signal_subscription*
catnip_signal_find_subscription(
  struct _catnip_signal* signal,
  catnip_signal_callback callback
)
{
  size_t subscriptions_length = catnip_vector_get_length(signal->subscriptions);

  for (size_t i = 0; i < subscriptions_length; ++i) {
    struct _catnip_signal_subscription* subscription =
      catnip_vector_at(signal->subscriptions, i);

    if (subscription->callback == callback) {
      return subscription;
    }
  }

  return NULL;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct _catnip_signal*
catnip_signal_create()
{
  struct _catnip_signal* signal = malloc(sizeof(struct _catnip_signal));

  return signal;
}

void
catnip_signal_destroy(struct _catnip_signal* signal)
{
  size_t subscriptions_length = catnip_vector_get_length(signal->subscriptions);

  for (size_t i = 0; i < subscriptions_length; ++i) {
    struct _catnip_signal_subscription* subscription =
      catnip_vector_at(signal->subscriptions, i);
    catnip_vector_remove(signal->subscriptions, subscription);
    free(subscription);
  }

  catnip_vector_destroy(signal->subscriptions);
  free(signal);
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_signal_subscribe(
  struct _catnip_signal* signal,
  catnip_signal_callback callback,
  void* invariant
)
{
  if (catnip_signal_find_subscription(signal, callback) != NULL) {
    return; // already added
  }

  struct _catnip_signal_subscription* subscription =
    malloc(sizeof(struct _catnip_signal_subscription));

  subscription->callback = callback;
  subscription->invariant = invariant;

  catnip_vector_add(signal->subscriptions, subscription);
}

void
catnip_signal_unsubscribe(
  struct _catnip_signal* signal,
  catnip_signal_callback callback
)
{
  struct _catnip_signal_subscription* subscription =
    catnip_signal_find_subscription(signal, callback);

  if (subscription == NULL) {
    return; // nothing to do
  }

  catnip_vector_remove(signal->subscriptions, subscription);
  free(subscription);
}

void
catnip_signal_publish(struct _catnip_signal* signal, void* data)
{
  size_t subscriptions_length = catnip_vector_get_length(signal->subscriptions);

  for (size_t i = 0; i < subscriptions_length; ++i) {
    struct _catnip_signal_subscription* subscription =
      catnip_vector_at(signal->subscriptions, i);

    subscription->callback(subscription->invariant, data);
  }
}
