#include "seat.h"
#include "backend.h"
#include "cursor.h"
#include "display.h"
#include "inputs/keyboard.h"
#include <wlr/types/wlr_data_device.h>

struct wlr_seat* server_seat;

static struct wl_listener seat_new_input_listener;
static struct wl_listener seat_request_cursor_listener;
static struct wl_listener seat_request_set_selection_listener;

static void
seat_new_input_notify(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
      register_new_keyboard(device);
      break;
    case WLR_INPUT_DEVICE_POINTER:
      /* We don't do anything special with pointers. All of our pointer handling
       * is proxied through wlr_cursor. On another compositor, you might take
       * this opportunity to do libinput configuration on the device to set
       * acceleration, etc. */
      wlr_cursor_attach_input_device(server_cursor, device);
      break;
    default:
      break;
  }

  /* We need to let the wlr_seat know what our capabilities are, which is
   * communiciated to the client. In TinyWL we always have a cursor, even if
   * there are no pointer devices, so we always include that capability. */
  uint32_t capabilities = WL_SEAT_CAPABILITY_POINTER;

  if (!wl_list_empty(&server_keyboards)) {
    capabilities |= WL_SEAT_CAPABILITY_KEYBOARD;
  }

  wlr_seat_set_capabilities(server_seat, capabilities);
}

static void
seat_request_set_cursor_notify(struct wl_listener* listener, void* data)
{
  struct wlr_seat_pointer_request_set_cursor_event* event = data;

  struct wlr_seat_client* focused_client =
    server_seat->pointer_state.focused_client;

  /* This can be sent by any client, so we check to make sure this one is
   * actually has pointer focus first. */
  if (focused_client == event->seat_client) {
    /* Once we've vetted the client, we can tell the cursor to use the
     * provided surface as the cursor image. It will set the hardware cursor
     * on the output that it's currently on and continue to do so as the
     * cursor moves between outputs. */
    wlr_cursor_set_surface(
      server_cursor,
      event->surface,
      event->hotspot_x,
      event->hotspot_y
    );
  }
}

static void
seat_request_set_selection_notify(struct wl_listener* listener, void* data)
{
  struct wlr_seat_request_set_selection_event* event = data;
  wlr_seat_set_selection(server_seat, event->source, event->serial);
}

void
init_server_seat()
{
  server_seat = wlr_seat_create(server_display, "seat0");

  init_server_keyboard();

  seat_new_input_listener.notify = seat_new_input_notify;
  wl_signal_add(&server_backend->events.new_input, &seat_new_input_listener);
  seat_request_cursor_listener.notify = seat_request_set_cursor_notify;
  wl_signal_add(
    &server_seat->events.request_set_cursor,
    &seat_request_cursor_listener
  );
  seat_request_set_selection_listener.notify =
    seat_request_set_selection_notify;
  wl_signal_add(
    &server_seat->events.request_set_selection,
    &seat_request_set_selection_listener
  );
}
