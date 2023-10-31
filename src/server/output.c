#include "output.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/output_layout.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "utils/log.h"
#include "utils/wayland.h"
#include <stdlib.h>

struct wl_list server_outputs;
struct server_output_events server_output_events;

static struct {
  struct wl_listener new_wlr_output;
} listeners;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static struct wlr_output_mode*
server_output_get_matching_mode(
  struct server_output* output,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* mode;

  wl_list_for_each(mode, &output->wlr_output->modes, link)
  {
    if (mode->width == width && mode->height == height && mode->refresh == refresh) {
      return mode;
    }
  }

  return NULL;
}

static void
server_output_set_matching_mode(
  struct server_output* output,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* matching_mode =
    server_output_get_matching_mode(output, width, height, refresh);

  if (matching_mode != NULL) {
    wlr_output_set_mode(output->wlr_output, matching_mode);
  } else {
    wlr_output_set_custom_mode(output->wlr_output, width, height, refresh);
  }
}

// -----------------------------------------------------------------------------
// Server Output
// -----------------------------------------------------------------------------

static void
on_wlr_output_frame(struct wl_listener* listener, void* data)
{
  struct server_output* output =
    wl_container_of(listener, output, listeners.frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(server_scene, output->wlr_output);

  wlr_scene_output_commit(scene_output);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
on_wlr_output_destroy(struct wl_listener* listener, void* data)
{
  struct server_output* output =
    wl_container_of(listener, output, listeners.destroy);

  wl_signal_emit_mutable(&output->events.destroy, output);

  wl_list_remove(&output->listeners.frame.link);
  wl_list_remove(&output->listeners.destroy.link);

  free(output);
}

static void
server_output_create(struct wlr_output* wlr_output)
{
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

  wlr_output_layout_add_auto(server_output_layout, wlr_output);

  struct server_output* output = calloc(1, sizeof(struct server_output));

  output->wlr_output = wlr_output;

  wl_signal_init(&output->events.destroy);

  wl_setup_listener(
    &output->listeners.frame,
    &wlr_output->events.frame,
    on_wlr_output_frame
  );
  wl_setup_listener(
    &output->listeners.destroy,
    &wlr_output->events.destroy,
    on_wlr_output_destroy
  );

  wl_list_insert(&server_outputs, &output->link);
  wl_signal_emit_mutable(&server_output_events.new_server_output, output);
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
server_output_get_x(struct server_output* output)
{
  return wlr_output_layout_get(server_output_layout, output->wlr_output)->x;
}

int
server_output_get_y(struct server_output* output)
{
  return wlr_output_layout_get(server_output_layout, output->wlr_output)->y;
}

int
server_output_get_width(struct server_output* output)
{
  return output->wlr_output->width;
}

int
server_output_get_height(struct server_output* output)
{
  return output->wlr_output->height;
}

int
server_output_get_refresh(struct server_output* output)
{
  return output->wlr_output->refresh;
}

struct wlr_output_mode*
server_output_get_mode(struct server_output* output)
{
  return output->wlr_output->current_mode;
}

struct wl_list*
server_output_get_modes(struct server_output* output)
{
  return &output->wlr_output->modes;
}

float
server_output_get_scale(struct server_output* output)
{
  return output->wlr_output->scale;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
server_output_set_x(struct server_output* output, int new_x)
{
  wlr_output_layout_move(
    server_output_layout,
    output->wlr_output,
    new_x,
    server_output_get_y(output)
  );
}

void
server_output_set_y(struct server_output* output, int new_y)
{
  wlr_output_layout_move(
    server_output_layout,
    output->wlr_output,
    server_output_get_x(output),
    new_y
  );
}

void
server_output_set_width(struct server_output* output, int new_width)
{
  server_output_set_matching_mode(
    output,
    new_width,
    server_output_get_height(output),
    server_output_get_refresh(output)
  );
}

void
server_output_set_height(struct server_output* output, int new_height)
{
  server_output_set_matching_mode(
    output,
    server_output_get_width(output),
    new_height,
    server_output_get_refresh(output)
  );
}

void
server_output_set_refresh(struct server_output* output, int new_refresh)
{
  server_output_set_matching_mode(
    output,
    server_output_get_width(output),
    server_output_get_height(output),
    new_refresh
  );
}

void
server_output_set_mode(
  struct server_output* output,
  struct wlr_output_mode* new_mode
)
{
  wlr_output_set_mode(output->wlr_output, new_mode);
}

void
server_output_set_scale(struct server_output* output, float new_scale)
{
  wlr_output_set_scale(output->wlr_output, new_scale);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_new_wlr_output(struct wl_listener* listener, void* data)
{
  server_output_create(data);
}

void
server_output_init()
{
  wl_list_init(&server_outputs);

  wl_signal_init(&server_output_events.new_server_output);

  wl_setup_listener(
    &listeners.new_wlr_output,
    &server_backend->events.new_output,
    on_new_wlr_output
  );
}
