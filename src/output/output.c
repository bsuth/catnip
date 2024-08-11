#include "output.h"
#include "config.h"
#include "extensions/wayland.h"
#include "id.h"
#include "output/lua_output.h"
#include "state/allocator.h"
#include "state/cursor.h"
#include "state/outputs.h"
#include "state/renderer.h"
#include "state/scene.h"
#include <stdlib.h>

static void
on_output_frame(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, frame_listener);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(catnip_scene, output->wlr_output);

  wlr_scene_output_commit(scene_output, NULL);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
on_output_request_state(struct wl_listener* listener, void* data)
{
  struct wlr_output_event_request_state* event = data;
  wlr_output_commit_state(event->output, event->state);
}

static void
on_output_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, destroy_listener);

  lua_catnip_output_destroy(catnip_L, output->lua_resource);

  wl_list_remove(&output->link);
  wl_list_remove(&output->frame_listener.link);
  wl_list_remove(&output->request_state_listener.link);
  wl_list_remove(&output->destroy_listener.link);

  wlr_output_layout_remove(catnip_output_layout, output->wlr_output);
  wlr_scene_output_destroy(output->scene_output);

  free(output);
}

struct catnip_output*
catnip_output_create(struct wlr_output* wlr_output)
{
  wlr_output_init_render(wlr_output, catnip_allocator, catnip_renderer);

  struct wlr_output_state state = {0};
  wlr_output_state_set_enabled(&state, true);

  struct wlr_output_mode* preferred_mode =
    wlr_output_preferred_mode(wlr_output);

  if (preferred_mode != NULL) {
    wlr_output_state_set_mode(&state, preferred_mode);
  }

  wlr_output_commit_state(wlr_output, &state);
  wlr_output_state_finish(&state);

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

  wl_signal_subscribe(
    &wlr_output->events.frame,
    &output->frame_listener,
    on_output_frame
  );
  wl_signal_subscribe(
    &wlr_output->events.request_state,
    &output->request_state_listener,
    on_output_request_state
  );
  wl_signal_subscribe(
    &wlr_output->events.destroy,
    &output->destroy_listener,
    on_output_destroy
  );

  lua_catnip_output_create(catnip_L, output);

  // Ensure we have loaded a scaled cursor theme for the new output's scale
  wlr_xcursor_manager_load(catnip_xcursor_manager, wlr_output->scale);

  return output;
}

void
catnip_output_configure(
  struct catnip_output* output,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_state state = {0};

  wlr_output_state_set_custom_mode(
    &state,
    width != -1 ? width : output->wlr_output->width,
    height != -1 ? height : output->wlr_output->height,
    refresh != -1 ? refresh : output->wlr_output->refresh
  );

  wlr_output_commit_state(output->wlr_output, &state);
  wlr_output_state_finish(&state);
}
