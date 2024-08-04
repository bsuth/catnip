#include "seat.h"
#include "backend.h"
#include "display.h"
#include "extensions/wayland.h"
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>

struct wlr_seat* catnip_seat = NULL;

struct wl_listener new_input_listener;
struct wl_listener request_set_selection_listener;

static int keyboard_capability_counter = 0;
static int pointer_capability_counter = 0;
static int touch_capability_counter = 0;

static int
get_device_capabilities(struct wlr_input_device* device)
{
  // TODO: Support other device types?
  // For now we just support pointers and keyboards, since they are the most
  // common cases and it's not clear at the moment how much work is required to
  // support other device types.
  if (device->type == WLR_INPUT_DEVICE_POINTER) {
    return WL_SEAT_CAPABILITY_POINTER;
  } else if (device->type == WLR_INPUT_DEVICE_KEYBOARD) {
    return WL_SEAT_CAPABILITY_KEYBOARD;
  } else {
    return 0;
  }
}

static void
update_seat_capabilities()
{
  wlr_seat_set_capabilities(
    catnip_seat,
    (keyboard_capability_counter > 0 ? WL_SEAT_CAPABILITY_KEYBOARD : 0)
      | (pointer_capability_counter > 0 ? WL_SEAT_CAPABILITY_POINTER : 0)
      | (touch_capability_counter > 0 ? WL_SEAT_CAPABILITY_TOUCH : 0)
  );
}

static void
register_device_capabilities(struct wlr_input_device* device)
{
  int capabilities = get_device_capabilities(device);
  keyboard_capability_counter += !!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD);
  pointer_capability_counter += !!(capabilities & WL_SEAT_CAPABILITY_POINTER);
  touch_capability_counter += !!(capabilities & WL_SEAT_CAPABILITY_TOUCH);
  update_seat_capabilities();
}

static void
unregister_capabilities(struct wlr_input_device* device)
{
  int capabilities = get_device_capabilities(device);
  keyboard_capability_counter -= !!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD);
  pointer_capability_counter -= !!(capabilities & WL_SEAT_CAPABILITY_POINTER);
  touch_capability_counter -= !!(capabilities & WL_SEAT_CAPABILITY_TOUCH);
  update_seat_capabilities();
}

static void
on_input_destroy(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;
  unregister_capabilities(device);
  wl_list_remove(&listener->link);
  free(listener);
}

static void
on_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  register_device_capabilities(device);

  struct wl_listener* destroy_listener = calloc(1, sizeof(struct wl_listener));

  wl_signal_subscribe(
    &device->events.destroy,
    destroy_listener,
    on_input_destroy
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

  wl_signal_subscribe(
    &catnip_backend->events.new_input,
    &new_input_listener,
    on_new_input
  );
  wl_signal_subscribe(
    &catnip_seat->events.request_set_selection,
    &request_set_selection_listener,
    on_request_set_selection
  );
}
