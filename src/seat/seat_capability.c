#include "seat_capability.h"
#include "seat.h"
#include "utils/wayland.h"
#include <stdlib.h>

static struct {
  int keyboard;
  int pointer;
  int touch;
} catnip_seat_capability_counters;

static void
catnip_seat_capability_update()
{
  int new_seat_capability = 0;

  if (catnip_seat_capability_counters.keyboard > 0) {
    new_seat_capability |= WL_SEAT_CAPABILITY_KEYBOARD;
  }

  if (catnip_seat_capability_counters.pointer > 0) {
    new_seat_capability |= WL_SEAT_CAPABILITY_POINTER;
  }

  if (catnip_seat_capability_counters.touch > 0) {
    new_seat_capability |= WL_SEAT_CAPABILITY_TOUCH;
  }

  // Fire this unconditionally, since wlroots already checks if the new
  // capability is the same as the old one for us (noop).
  wlr_seat_set_capabilities(catnip_seat, new_seat_capability);
}

static void
catnip_seat_capability_pointer_destroy(struct wl_listener* listener, void* data)
{
  --catnip_seat_capability_counters.pointer;
  catnip_seat_capability_update();
  free(listener);
}

static void
catnip_seat_capability_keyboard_destroy(
  struct wl_listener* listener,
  void* data
)
{
  --catnip_seat_capability_counters.keyboard;
  catnip_seat_capability_update();
  free(listener);
}

static void
catnip_seat_capability_touch_destroy(struct wl_listener* listener, void* data)
{
  --catnip_seat_capability_counters.touch;
  catnip_seat_capability_update();
  free(listener);
}

void
catnip_seat_capability_register_pointer(struct wlr_input_device* device)
{
  ++catnip_seat_capability_counters.pointer;
  catnip_seat_capability_update();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));

  wl_setup_listener(
    on_destroy,
    &device->events.destroy,
    catnip_seat_capability_pointer_destroy
  );
}

void
catnip_seat_capability_register_keyboard(struct wlr_input_device* device)
{
  ++catnip_seat_capability_counters.keyboard;
  catnip_seat_capability_update();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));

  wl_setup_listener(
    on_destroy,
    &device->events.destroy,
    catnip_seat_capability_keyboard_destroy
  );
}

void
catnip_seat_capability_register_touch(struct wlr_input_device* device)
{
  ++catnip_seat_capability_counters.touch;
  catnip_seat_capability_update();

  struct wl_listener* on_destroy = calloc(1, sizeof(struct wl_listener));

  wl_setup_listener(
    on_destroy,
    &device->events.destroy,
    catnip_seat_capability_touch_destroy
  );
}
