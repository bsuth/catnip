#include "cursor.h"
#include "backend.h"
#include "input/seat.h"
#include "output/output_layout.h"
#include "scene.h"
#include "utils/time.h"
#include "utils/wayland.h"
#include <stdlib.h>
#include <string.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

struct catnip_cursor {
  char* name;

  struct wlr_cursor* wlr_cursor;
  struct wlr_xcursor_manager* wlr_xcursor_manager;

  struct {
    struct wl_listener motion;
    struct wl_listener motion_absolute;
    struct wl_listener button;
    struct wl_listener axis;
    struct wl_listener frame;
    struct wl_listener request_set_cursor;
    struct wl_listener new_output;
  } listeners;
};

struct catnip_cursor* catnip_cursor = NULL;

static void
catnip_cursor_notify(uint32_t time_msec)
{
  double sx = 0;
  double sy = 0;

  struct wlr_scene_node* target_node = wlr_scene_node_at(
    &catnip_scene->tree.node,
    catnip_cursor_get_x(),
    catnip_cursor_get_y(),
    &sx,
    &sy
  );

  if (target_node == NULL || target_node->type != WLR_SCENE_NODE_BUFFER) {
    wlr_seat_pointer_clear_focus(catnip_seat);
  } else {
    struct wlr_scene_buffer* target_buffer =
      wlr_scene_buffer_from_node(target_node);

    struct wlr_scene_surface* target_scene_surface =
      wlr_scene_surface_try_from_buffer(target_buffer);

    if (target_scene_surface != NULL) {
      wlr_seat_pointer_notify_enter(
        catnip_seat,
        target_scene_surface->surface,
        sx,
        sy
      );
    }

    wlr_seat_pointer_notify_motion(catnip_seat, time_msec, sx, sy);
  }
}

static void
catnip_cursor_motion(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_event* event = data;

  wlr_cursor_move(
    catnip_cursor->wlr_cursor,
    &event->pointer->base,
    event->delta_x,
    event->delta_y
  );

  catnip_cursor_notify(event->time_msec);
}

static void
catnip_cursor_motion_absolute(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_motion_absolute_event* event = data;

  wlr_cursor_warp_absolute(
    catnip_cursor->wlr_cursor,
    &event->pointer->base,
    event->x,
    event->y
  );

  catnip_cursor_notify(event->time_msec);
}

static void
catnip_cursor_button(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_button_event* event = data;

  wlr_seat_pointer_notify_button(
    catnip_seat,
    event->time_msec,
    event->button,
    event->state
  );
}

static void
catnip_cursor_axis(struct wl_listener* listener, void* data)
{
  struct wlr_pointer_axis_event* event = data;

  wlr_seat_pointer_notify_axis(
    catnip_seat,
    event->time_msec,
    event->orientation,
    event->delta,
    event->delta_discrete,
    event->source
  );
}

static void
catnip_cursor_frame(struct wl_listener* listener, void* data)
{
  wlr_seat_pointer_notify_frame(catnip_seat);
}

static void
catnip_cursor_request_set_cursor(struct wl_listener* listener, void* data)
{
  struct wlr_seat_pointer_request_set_cursor_event* event = data;

  if (event->seat_client != catnip_seat->pointer_state.focused_client) {
    return; // only accept cursor requests from the currently focused client.
  }

  wlr_cursor_set_surface(
    catnip_cursor->wlr_cursor,
    event->surface,
    event->hotspot_x,
    event->hotspot_y
  );
}

static void
catnip_cursor_new_output(struct wl_listener* listener, void* data)
{
  // Ensure we have loaded a scaled theme for the new output's scale
  struct wlr_output* wlr_output = data;
  wlr_xcursor_manager_load(
    catnip_cursor->wlr_xcursor_manager,
    wlr_output->scale
  );
}

static void
catnip_cursor_load_scales()
{
  // Always load scale 1
  wlr_xcursor_manager_load(catnip_cursor->wlr_xcursor_manager, 1);

  // Ensure we have loaded a scaled theme for each output's scale
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    wlr_xcursor_manager_load(
      catnip_cursor->wlr_xcursor_manager,
      output->wlr_output->scale
    );
  }
}

void
catnip_cursor_init()
{
  catnip_cursor = calloc(1, sizeof(struct catnip_cursor));

  catnip_cursor->wlr_cursor = wlr_cursor_create();
  catnip_cursor->wlr_xcursor_manager = wlr_xcursor_manager_create(NULL, 24);

  catnip_cursor_set_name("default");
  catnip_cursor_load_scales();

  wlr_cursor_attach_output_layout(
    catnip_cursor->wlr_cursor,
    catnip_output_layout
  );

  wl_setup_listener(
    &catnip_cursor->listeners.motion,
    &catnip_cursor->wlr_cursor->events.motion,
    catnip_cursor_motion
  );
  wl_setup_listener(
    &catnip_cursor->listeners.motion_absolute,
    &catnip_cursor->wlr_cursor->events.motion_absolute,
    catnip_cursor_motion_absolute
  );
  wl_setup_listener(
    &catnip_cursor->listeners.button,
    &catnip_cursor->wlr_cursor->events.button,
    catnip_cursor_button
  );
  wl_setup_listener(
    &catnip_cursor->listeners.axis,
    &catnip_cursor->wlr_cursor->events.axis,
    catnip_cursor_axis
  );
  wl_setup_listener(
    &catnip_cursor->listeners.frame,
    &catnip_cursor->wlr_cursor->events.frame,
    catnip_cursor_frame
  );
  wl_setup_listener(
    &catnip_cursor->listeners.request_set_cursor,
    &catnip_seat->events.request_set_cursor,
    catnip_cursor_request_set_cursor
  );
  wl_setup_listener(
    &catnip_cursor->listeners.new_output,
    &catnip_backend->events.new_output,
    catnip_cursor_new_output
  );
}

int
catnip_cursor_get_x()
{
  return catnip_cursor->wlr_cursor->x;
}

void
catnip_cursor_set_x(int new_x)
{
  wlr_cursor_warp_closest(
    catnip_cursor->wlr_cursor,
    NULL,
    new_x,
    catnip_cursor_get_y()
  );

  catnip_cursor_notify(get_time_msec());
}

int
catnip_cursor_get_y()
{
  return catnip_cursor->wlr_cursor->y;
}

void
catnip_cursor_set_y(int new_y)
{
  wlr_cursor_warp_closest(
    catnip_cursor->wlr_cursor,
    NULL,
    catnip_cursor_get_x(),
    new_y
  );

  catnip_cursor_notify(get_time_msec());
}

const char*
catnip_cursor_get_name()
{
  return catnip_cursor->name;
}

void
catnip_cursor_set_name(const char* new_name)
{
  if (catnip_cursor->name != NULL) {
    free(catnip_cursor->name);
    catnip_cursor->name = NULL;
  }

  if (new_name == NULL) {
    wlr_cursor_unset_image(catnip_cursor->wlr_cursor);
  } else {
    catnip_cursor->name = strdup(new_name);
    wlr_cursor_set_xcursor(
      catnip_cursor->wlr_cursor,
      catnip_cursor->wlr_xcursor_manager,
      new_name
    );
  }
}

uint32_t
catnip_cursor_get_size()
{
  return catnip_cursor->wlr_xcursor_manager->size;
}

void
catnip_cursor_set_size(uint32_t new_size)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(catnip_cursor_get_theme(), new_size);

  catnip_cursor_load_scales();

  wlr_xcursor_manager_destroy(catnip_cursor->wlr_xcursor_manager);
  catnip_cursor->wlr_xcursor_manager = new_wlr_xcursor_manager;
}

const char*
catnip_cursor_get_theme()
{
  return catnip_cursor->wlr_xcursor_manager->name;
}

void
catnip_cursor_set_theme(const char* new_theme)
{
  struct wlr_xcursor_manager* new_wlr_xcursor_manager =
    wlr_xcursor_manager_create(new_theme, catnip_cursor_get_size());

  catnip_cursor_load_scales();

  wlr_xcursor_manager_destroy(catnip_cursor->wlr_xcursor_manager);
  catnip_cursor->wlr_xcursor_manager = new_wlr_xcursor_manager;
}

void
catnip_cursor_attach_pointer(struct wlr_input_device* device)
{
  wlr_cursor_attach_input_device(catnip_cursor->wlr_cursor, device);
}
