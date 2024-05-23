#include "init.h"
#include "backend.h"
#include "output/output.h"
#include "output/output_layout.h"
#include "output/outputs.h"
#include "utils/wayland.h"

static struct wl_listener new_output_listener;

static void
on_new_output(struct wl_listener* listener, void* data)
{
  struct wlr_output* wlr_output = data;
  struct catnip_output* output = catnip_output_create(wlr_output);
  wl_list_insert(&catnip_outputs, &output->link);
}

void
catnip_output_init()
{
  catnip_output_layout_init();

  catnip_outputs_init();

  wl_setup_listener(
    &new_output_listener,
    &catnip_backend->events.new_output,
    on_new_output
  );
}
