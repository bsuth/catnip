#include "seat.h"
#include "backend.h"
#include "cursor/cursor.h"
#include "display.h"
#include "keyboard.h"
#include "utils/wayland.h"
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>

struct wlr_seat* catnip_seat;

static struct {
  struct wl_listener new_input;
  struct wl_listener request_set_cursor;
  struct wl_listener request_set_selection;
} listeners;

static void
on_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
      catnip_keyboard_create(device);
      break;
    case WLR_INPUT_DEVICE_POINTER:
      /* We don't do anything special with pointers. All of our pointer handling
       * is proxied through wlr_cursor. On another compositor, you might take
       * this opportunity to do libinput configuration on the device to set
       * acceleration, etc. */
      wlr_cursor_attach_input_device(catnip_cursor, device);
      break;
    default:
      break;
  }

  /* We need to let the wlr_seat know what our capabilities are, which is
   * communiciated to the client. In TinyWL we always have a cursor, even if
   * there are no pointer devices, so we always include that capability. */
  uint32_t capabilities = WL_SEAT_CAPABILITY_POINTER;

  if (!wl_list_empty(&catnip_keyboards)) {
    capabilities |= WL_SEAT_CAPABILITY_KEYBOARD;
  }

  wlr_seat_set_capabilities(catnip_seat, capabilities);
}

static void
on_request_set_cursor(struct wl_listener* listener, void* data)
{
  struct wlr_seat_pointer_request_set_cursor_event* event = data;

  struct wlr_seat_client* focused_client =
    catnip_seat->pointer_state.focused_client;

  /* This can be sent by any client, so we check to make sure this one is
   * actually has pointer focus first. */
  if (focused_client == event->seat_client) {
    /* Once we've vetted the client, we can tell the cursor to use the
     * provided surface as the cursor image. It will set the hardware cursor
     * on the output that it's currently on and continue to do so as the
     * cursor moves between outputs. */
    wlr_cursor_set_surface(
      catnip_cursor,
      event->surface,
      event->hotspot_x,
      event->hotspot_y
    );
  }
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

  catnip_keyboard_init();

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
