#include "monitor.h"
#include "lua_api/monitor.h"
#include "server/allocator.h"
#include "server/backend.h"
#include "server/output.h"
#include "server/renderer.h"
#include "server/scene.h"
#include "utils/wayland.h"
#include <stdlib.h>
#include <wayland-util.h>

static struct wl_listener server_backend_new_output_listener;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static struct wlr_output_mode*
desktop_window_get_matching_mode(
  struct desktop_monitor* monitor,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* mode;

  wl_list_for_each(mode, &monitor->output->modes, link)
  {
    if (mode->width == width && mode->height == height && mode->refresh == refresh) {
      return mode;
    }
  }

  return NULL;
}

static void
desktop_window_set_matching_mode(
  struct desktop_monitor* monitor,
  int width,
  int height,
  int refresh
)
{
  struct wlr_output_mode* matching_mode =
    desktop_window_get_matching_mode(monitor, width, height, refresh);

  if (matching_mode != NULL) {
    wlr_output_set_mode(monitor->output, matching_mode);
  } else {
    wlr_output_set_custom_mode(monitor->output, width, height, refresh);
  }
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

static void
desktop_monitor_output_frame(struct wl_listener* listener, void* data)
{
  struct desktop_monitor* monitor = wl_container_of(listener, monitor, frame);

  struct wlr_scene_output* scene_output =
    wlr_scene_get_scene_output(server_scene, monitor->output);

  wlr_scene_output_commit(scene_output);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

static void
desktop_monitor_output_destroy(struct wl_listener* listener, void* data)
{
  struct desktop_monitor* monitor = wl_container_of(listener, monitor, destroy);

  wl_list_remove(&monitor->frame.link);
  wl_list_remove(&monitor->destroy.link);

  free(monitor);
}

static void
create_desktop_monitor(struct wl_listener* listener, void* data)
{
  struct wlr_output* output = data;

  wlr_output_init_render(output, server_allocator, server_renderer);

  // Some backends don't have modes. DRM+KMS does, and we need to set a mode
  // before we can use the output. The mode is a tuple of (width, height,
  // refresh rate), and each monitor supports only a specific set of modes. We
  // just pick the monitor's preferred mode, a more sophisticated compositor
  // would let the user configure it.
  // TODO: let user configure this.
  if (!wl_list_empty(&output->modes)) {
    struct wlr_output_mode* mode = wlr_output_preferred_mode(output);

    wlr_output_set_mode(output, mode);
    wlr_output_enable(output, true);

    if (!wlr_output_commit(output)) {
      // TODO: log?
      return;
    }
  }

  struct desktop_monitor* monitor = calloc(1, sizeof(struct desktop_monitor));
  monitor->output = output;

  wl_setup_listener(
    &monitor->frame,
    &output->events.frame,
    desktop_monitor_output_frame
  );
  wl_setup_listener(
    &monitor->destroy,
    &output->events.destroy,
    desktop_monitor_output_destroy
  );

  // Adds this to the output layout. The add_auto function arranges outputs
  // from left-to-right in the order they appear. A more sophisticated
  // compositor would let the user configure the arrangement of outputs in the
  // layout.
  //
  // The output layout utility automatically adds a wl_output global to the
  // display, which Wayland clients can see to find out information about the
  // output (such as DPI, scale factor, manufacturer, etc).
  //
  // TODO allow layout configuration
  wlr_output_layout_add_auto(server_output_layout, output);

  create_lua_desktop_monitor(monitor);
}

void
init_desktop_monitors()
{
  wl_setup_listener(
    &server_backend_new_output_listener,
    &server_backend->events.new_output,
    create_desktop_monitor
  );
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int
desktop_monitor_get_x(struct desktop_monitor* monitor)
{
  return wlr_output_layout_get(server_output_layout, monitor->output)->x;
}

int
desktop_monitor_get_y(struct desktop_monitor* monitor)
{
  return wlr_output_layout_get(server_output_layout, monitor->output)->y;
}

int
desktop_monitor_get_width(struct desktop_monitor* monitor)
{
  return monitor->output->width;
}

int
desktop_monitor_get_height(struct desktop_monitor* monitor)
{
  return monitor->output->height;
}

int
desktop_monitor_get_refresh(struct desktop_monitor* monitor)
{
  return monitor->output->refresh;
}

struct wlr_output_mode*
desktop_monitor_get_mode(struct desktop_monitor* monitor)
{
  return monitor->output->current_mode;
}

struct wl_list
desktop_monitor_get_modes(struct desktop_monitor* monitor)
{
  return monitor->output->modes;
}

float
desktop_monitor_get_scale(struct desktop_monitor* monitor)
{
  return monitor->output->scale;
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void
desktop_monitor_set_x(struct desktop_monitor* monitor, int new_x)
{
  wlr_output_layout_move(
    server_output_layout,
    monitor->output,
    new_x,
    desktop_monitor_get_y(monitor)
  );
}

void
desktop_monitor_set_y(struct desktop_monitor* monitor, int new_y)
{
  wlr_output_layout_move(
    server_output_layout,
    monitor->output,
    desktop_monitor_get_x(monitor),
    new_y
  );
}

void
desktop_monitor_set_width(struct desktop_monitor* monitor, int new_width)
{
  desktop_window_set_matching_mode(
    monitor,
    new_width,
    desktop_monitor_get_height(monitor),
    desktop_monitor_get_refresh(monitor)
  );
}

void
desktop_monitor_set_height(struct desktop_monitor* monitor, int new_height)
{
  desktop_window_set_matching_mode(
    monitor,
    desktop_monitor_get_width(monitor),
    new_height,
    desktop_monitor_get_refresh(monitor)
  );
}

void
desktop_monitor_set_refresh(struct desktop_monitor* monitor, int new_refresh)
{
  desktop_window_set_matching_mode(
    monitor,
    desktop_monitor_get_width(monitor),
    desktop_monitor_get_height(monitor),
    new_refresh
  );
}

void
desktop_monitor_set_mode(
  struct desktop_monitor* monitor,
  struct wlr_output_mode* new_mode
)
{
  wlr_output_set_mode(monitor->output, new_mode);
}

void
desktop_monitor_set_scale(struct desktop_monitor* monitor, float new_scale)
{
  wlr_output_set_scale(monitor->output, new_scale);
}
