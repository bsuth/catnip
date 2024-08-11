#include "outputs.h"
#include "extensions/wayland.h"
#include "output/output.h"
#include "state/backend.h"

struct wl_list catnip_outputs;

static struct {
  struct wl_listener backend_new_output;
} listeners;

static void
on_backend_new_output(struct wl_listener* listener, void* data)
{
  struct wlr_output* wlr_output = data;
  struct catnip_output* output = catnip_output_create(wlr_output);
  wl_list_insert(&catnip_outputs, &output->link);
}

void
catnip_outputs_init()
{
  wl_list_init(&catnip_outputs);

  wl_signal_subscribe(
    &catnip_backend->events.new_output,
    &listeners.backend_new_output,
    on_backend_new_output
  );
}
