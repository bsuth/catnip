#include "output.h"
#include "allocator.h"
#include "backend.h"
#include "config.h"
#include "cursor/cursor.h"
#include "id.h"
#include "output/lua_output.h"
#include "output/output_layout.h"
#include "renderer.h"
#include "scene.h"
#include "utils/wayland.h"
#include <stdlib.h>

struct wl_list catnip_outputs;

static struct {
  struct wl_listener new_output;
} listeners;

static void
catnip_output_frame(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(catnip_scene, output->wlr_output);

  wlr_scene_output_commit(scene_output, NULL);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
catnip_output_request_state(struct wl_listener* listener, void* data)
{
  struct wlr_output_event_request_state* event = data;
  wlr_output_commit_state(event->output, event->state);
}

static void
catnip_output_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.destroy);

  lua_catnip_output_destroy(catnip_L, output->lua_resource);

  wl_list_remove(&output->link);
  wl_list_remove(&output->listeners.frame.link);
  wl_list_remove(&output->listeners.destroy.link);

  wlr_output_layout_remove(catnip_output_layout, output->wlr_output);
  wlr_scene_output_destroy(output->scene_output);

  free(output);
}

static void
catnip_output_create(struct wl_listener* listener, void* data)
{
  struct wlr_output* wlr_output = data;

  wlr_output_init_render(wlr_output, catnip_allocator, catnip_renderer);
  wlr_output_enable(wlr_output, true);

  struct wlr_output_mode* preferred_mode =
    wlr_output_preferred_mode(wlr_output);

  if (preferred_mode != NULL) {
    wlr_output_set_mode(wlr_output, preferred_mode);
  }

  wlr_output_commit(wlr_output);

  struct catnip_output* output = calloc(1, sizeof(struct catnip_output));

  output->id = generate_catnip_id();
  output->wlr_output = wlr_output;
  output->layout_output =
    wlr_output_layout_add_auto(catnip_output_layout, wlr_output);
  output->scene_output = wlr_scene_output_create(catnip_scene, wlr_output);

  wlr_scene_output_layout_add_output(
    catnip_scene_output_layout,
    output->layout_output,
    output->scene_output
  );

  wl_setup_listener(
    &output->listeners.frame,
    &wlr_output->events.frame,
    catnip_output_frame
  );
  wl_setup_listener(
    &output->listeners.request_state,
    &wlr_output->events.request_state,
    catnip_output_request_state
  );
  wl_setup_listener(
    &output->listeners.destroy,
    &wlr_output->events.destroy,
    catnip_output_destroy
  );

  wl_list_insert(&catnip_outputs, &output->link);

  lua_catnip_output_create(catnip_L, output);

  // Ensure we have loaded a scaled cursor theme for the new output's scale
  wlr_xcursor_manager_load(catnip_xcursor_manager, wlr_output->scale);
}

void
catnip_output_init()
{
  catnip_output_layout_init();

  wl_list_init(&catnip_outputs);

  wl_setup_listener(
    &listeners.new_output,
    &catnip_backend->events.new_output,
    catnip_output_create
  );
}
