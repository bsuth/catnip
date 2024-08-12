#include "keyboards.h"
#include "compositor/backend.h"
#include "desktop/keyboard.h"
#include "extensions/wayland.h"

struct wl_list catnip_keyboards;

static struct {
  struct wl_listener backend_new_input;
} listeners;

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_backend_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_KEYBOARD) {
    return;
  }

  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);
  struct catnip_keyboard* keyboard = catnip_keyboard_create(wlr_keyboard);
  wl_list_insert(&catnip_keyboards, &keyboard->link);
}

void
catnip_keyboards_init()
{
  wl_list_init(&catnip_keyboards);

  wl_signal_subscribe(
    &catnip_backend->events.new_input,
    &listeners.backend_new_input,
    on_backend_new_input
  );
}
