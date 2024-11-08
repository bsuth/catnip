#include "output.h"
#include "compositor/allocator.h"
#include "compositor/renderer.h"
#include "compositor/scene.h"
#include "config.h"
#include "desktop/cursor.h"
#include "desktop/lua_output.h"
#include "desktop/outputs.h"
#include "extensions/wayland.h"
#include "id.h"
#include "widget/lua_widget_root.h"
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Create
// -----------------------------------------------------------------------------

static void
on_output_frame(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.output_frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(catnip_scene, output->wlr.output);

  wlr_scene_output_commit(scene_output, NULL);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
on_output_request_state(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.output_request_state);

  struct wlr_output_event_request_state* event = data;
  wlr_output_commit_state(event->output, event->state);

  if (event->state->mode != NULL) {
    int32_t new_width = event->state->mode->width;
    int32_t new_height = event->state->mode->height;

    bool needs_wallpaper_resize = output->wlr.output->width != new_width
      || output->wlr.output->height != new_height && output->lua_output != NULL;

    if (needs_wallpaper_resize) {
      lua_rawgeti(catnip_L, LUA_REGISTRYINDEX, output->lua_output->wallpaper);
      struct catnip_lua_widget_root* root = lua_popuserdata(catnip_L);
      catnip_lua_widget_root_request_resize(root, new_width, new_height);
    }
  }
}

static void
on_output_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_output* output =
    wl_container_of(listener, output, listeners.output_destroy);

  catnip_lua_output_destroy(catnip_L, output->lua_output);

  wl_list_remove(&output->link);
  wl_list_remove(&output->listeners.output_frame.link);
  wl_list_remove(&output->listeners.output_request_state.link);
  wl_list_remove(&output->listeners.output_destroy.link);

  wlr_output_layout_remove(catnip_output_layout, output->wlr.output);
  wlr_scene_output_destroy(output->wlr.scene_output);

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
  output->wlr.output = wlr_output;
  output->wlr.output_layout_output =
    wlr_output_layout_add_auto(catnip_output_layout, wlr_output);
  output->wlr.scene_output = wlr_scene_output_create(catnip_scene, wlr_output);

  wlr_scene_output_layout_add_output(
    catnip_scene_output_layout,
    output->wlr.output_layout_output,
    output->wlr.scene_output
  );

  wl_signal_subscribe(
    &wlr_output->events.frame,
    &output->listeners.output_frame,
    on_output_frame
  );
  wl_signal_subscribe(
    &wlr_output->events.request_state,
    &output->listeners.output_request_state,
    on_output_request_state
  );
  wl_signal_subscribe(
    &wlr_output->events.destroy,
    &output->listeners.output_destroy,
    on_output_destroy
  );

  catnip_lua_output_create(catnip_L, output);

  // Ensure we have loaded a scaled cursor theme for the new output's scale
  wlr_xcursor_manager_load(catnip_xcursor_manager, wlr_output->scale);

  return output;
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

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
    width != -1 ? width : output->wlr.output->width,
    height != -1 ? height : output->wlr.output->height,
    refresh != -1 ? refresh : output->wlr.output->refresh
  );

  wlr_output_commit_state(output->wlr.output, &state);
  wlr_output_state_finish(&state);
}
