#include "outputs.h"
#include "compositor/backend.h"
#include "compositor/display.h"
#include "compositor/scene.h"
#include "desktop/output.h"
#include "extensions/wayland.h"

struct wl_list catnip_outputs;
struct wlr_output_layout* catnip_output_layout = NULL;
struct wlr_scene_output_layout* catnip_scene_output_layout = NULL;

static struct {
  struct wl_listener backend_new_output;
} listeners;

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

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

  catnip_output_layout = wlr_output_layout_create(catnip_display);
  catnip_scene_output_layout =
    wlr_scene_attach_output_layout(catnip_scene, catnip_output_layout);

  wl_signal_subscribe(
    &catnip_backend->events.new_output,
    &listeners.backend_new_output,
    on_backend_new_output
  );
}
