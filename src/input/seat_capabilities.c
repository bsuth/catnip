#include "seat_capabilities.h"
#include "cursor/cursor.h"
#include "seat.h"
#include "utils/time.h"
#include "utils/wayland.h"
#include <stdlib.h>

static struct {
  int keyboard;
  int pointer;
  int touch;
} capability_counters;

static void
update_capabilities()
{
  // Fire this unconditionally, since wlroots already checks if the new
  // capability is the same as the old one for us (noop).
  wlr_seat_set_capabilities(
    catnip_seat,
    (capability_counters.keyboard > 0 && WL_SEAT_CAPABILITY_KEYBOARD)
      | (capability_counters.pointer > 0 && WL_SEAT_CAPABILITY_POINTER)
      | (capability_counters.touch > 0 && WL_SEAT_CAPABILITY_TOUCH)
  );
}

static void
on_pointer_destroy(struct wl_listener* listener, void* data)
{
  --capability_counters.pointer;
  update_capabilities();
  free(listener);

  if (capability_counters.pointer == 0) {
    // no more pointer devices, hide the cursor
    wlr_cursor_set_buffer(catnip_cursor, NULL, 0, 0, 0);
  }
}

static void
on_keyboard_destroy(struct wl_listener* listener, void* data)
{
  --capability_counters.keyboard;
  update_capabilities();
  free(listener);
}

static void
on_touch_destroy(struct wl_listener* listener, void* data)
{
  --capability_counters.touch;
  update_capabilities();
  free(listener);
}

void
catnip_seat_capability_register_pointer(struct wlr_input_device* device)
{
  ++capability_counters.pointer;
  update_capabilities();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));
  wl_setup_listener(on_destroy, &device->events.destroy, on_pointer_destroy);

  if (capability_counters.pointer == 1) {
    // first pointer appeared, show the cursor
    catnip_cursor_update(get_time_msec());
  }
}

void
catnip_seat_capability_register_keyboard(struct wlr_input_device* device)
{
  ++capability_counters.keyboard;
  update_capabilities();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));
  wl_setup_listener(on_destroy, &device->events.destroy, on_keyboard_destroy);
}

void
catnip_seat_capability_register_touch(struct wlr_input_device* device)
{
  ++capability_counters.touch;
  update_capabilities();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));
  wl_setup_listener(on_destroy, &device->events.destroy, on_touch_destroy);
}
