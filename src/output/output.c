#include "output.h"
#include "allocator.h"
#include "config.h"
#include "cursor/cursor.h"
#include "id.h"
#include "output/lua_output.h"
#include "output/output_layout.h"
#include "renderer.h"
#include "scene.h"
#include "utils/wayland.h"
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
    &output->frame_listener,
    &wlr_output->events.frame,
    on_output_frame
  );
  wl_setup_listener(
    &output->request_state_listener,
    &wlr_output->events.request_state,
    on_output_request_state
  );
  wl_setup_listener(
    &output->destroy_listener,
    &wlr_output->events.destroy,
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
  wlr_output_set_custom_mode(
    output->wlr_output,
    width != -1 ? width
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.width
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->width
      : output->wlr_output->width,
    height != -1 ? height
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.height
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->height
      : output->wlr_output->height,
    refresh != -1 ? refresh
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.refresh
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->refresh
      : output->wlr_output->refresh
  );
}
