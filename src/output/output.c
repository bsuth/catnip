#include "output.h"
#include "config/config.h"
#include "output/lua_output.h"
#include "output/output_layout.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "utils/log.h"
#include "utils/wayland.h"
#include <stdlib.h>

struct wl_list catnip_outputs;

static struct {
  struct wl_listener new_wlr_output;
} listeners;

static void
on_frame(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(server_scene, output->wlr_output);

  wlr_scene_output_commit(scene_output);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
on_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.destroy);

  if (L != NULL) {
    lua_catnip_output_destroy(L, output);
  }

  wl_list_remove(&output->listeners.frame.link);
  wl_list_remove(&output->listeners.destroy.link);

  free(output);
}

static void
catnip_output_create(struct wl_listener* listener, void* data)
{
  struct wlr_output* wlr_output = data;

  wlr_output_init_render(wlr_output, server_allocator, server_renderer);

  if (!wl_list_empty(&wlr_output->modes)) {
    // Use the preferred mode as the default.
    struct wlr_output_mode* mode = wlr_output_preferred_mode(wlr_output);

    wlr_output_set_mode(wlr_output, mode);
    wlr_output_enable(wlr_output, true);

    if (!wlr_output_commit(wlr_output)) {
      log_error("failed to init output %s", wlr_output->name);
      return;
    }
  }

  wlr_output_layout_add_auto(catnip_output_layout, wlr_output);

  struct catnip_output* output = calloc(1, sizeof(struct catnip_output));

  output->wlr_output = wlr_output;

  wl_setup_listener(
    &output->listeners.frame,
    &wlr_output->events.frame,
    on_frame
  );
  wl_setup_listener(
    &output->listeners.destroy,
    &wlr_output->events.destroy,
    on_destroy
  );

  wl_list_insert(&catnip_outputs, &output->link);

  if (L != NULL) {
    lua_catnip_output_create(L, output);
  }
}

void
catnip_output_init()
{
  catnip_output_layout_init();

  wl_list_init(&catnip_outputs);

  wl_setup_listener(
    &listeners.new_wlr_output,
    &server_backend->events.new_output,
    catnip_output_create
  );
}
