#include "seat.h"
#include "backend.h"
#include "cursor/cursor.h"
#include "display.h"
#include "keyboard.h"
#include "seat_capabilities.h"
#include "utils/wayland.h"
#include <wayland-util.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>

struct wlr_seat* catnip_seat = NULL;

static struct {
  struct wl_listener new_input;
  struct wl_listener request_set_cursor;
  struct wl_listener request_set_selection;
} listeners;

static void
on_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  // TODO: Support other device types?
  // For now we just support pointers and keyboards, since they are the most
  // common cases and it's not clear at the moment how much work is required to
  // support other device types.
  if (device->type == WLR_INPUT_DEVICE_POINTER) {
    catnip_seat_capability_register_pointer(device);
    wlr_cursor_attach_input_device(catnip_cursor, device);
  } else if (device->type == WLR_INPUT_DEVICE_KEYBOARD) {
    catnip_seat_capability_register_keyboard(device);
    catnip_keyboard_create(device);
  }
}

static void
on_request_set_cursor(struct wl_listener* listener, void* data)
{
  struct wlr_seat_pointer_request_set_cursor_event* event = data;

  if (event->seat_client != catnip_seat->pointer_state.focused_client) {
    return; // only accept cursor requests from the currently focused client.
  }

  wlr_cursor_set_surface(
    catnip_cursor,
    event->surface,
    event->hotspot_x,
    event->hotspot_y
  );
}

static void
on_request_set_selection(struct wl_listener* listener, void* data)
{
  struct wlr_seat_request_set_selection_event* event = data;
  wlr_seat_set_selection(catnip_seat, event->source, event->serial);
}

void
catnip_seat_init()
{
  catnip_seat = wlr_seat_create(catnip_display, "seat0");

  wl_setup_listener(
    &listeners.new_input,
    &catnip_backend->events.new_input,
    on_new_input
  );
  wl_setup_listener(
    &listeners.request_set_cursor,
    &catnip_seat->events.request_set_cursor,
    on_request_set_cursor
  );
  wl_setup_listener(
    &listeners.request_set_selection,
    &catnip_seat->events.request_set_selection,
    on_request_set_selection
  );
}
