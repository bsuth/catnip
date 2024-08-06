#include "keyboards.h"
#include "backend.h"
#include "extensions/wayland.h"
#include "objects/keyboard.h"
#include "objects/vector.h"

struct catnip_keyboards_state catnip_keyboards_state = {
  .keyboards = NULL,
};

static struct {
  struct wl_listener backend_new_input;
} listeners;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
on_new_input(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_KEYBOARD) {
    return;
  }

  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  catnip_keyboard_t* keyboard = catnip_keyboard_create(wlr_keyboard);
  catnip_vector_add(catnip_keyboards_state.keyboards, keyboard);

  // TODO: emit immediately
  wl_signal_emit(&catnip_keyboards_state.signals.create, keyboard);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
catnip_keyboards_init()
{
  catnip_keyboards_state.keyboards = catnip_vector_create(3);

  wl_signal_init(&catnip_keyboards_state.signals.create);

  wl_signal_subscribe(
    &catnip_backend->events.new_input,
    &listeners.backend_new_input,
    on_new_input
  );
}
