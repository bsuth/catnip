#include "init.h"
#include "backend.h"
#include "extensions/wayland.h"
#include "keyboard/keyboard.h"
#include "keyboard/keyboards.h"

static struct wl_listener new_input_listener;

static void
on_new_input(struct wl_listener* listener, void* data)
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
catnip_keyboard_init()
{
  wl_list_init(&catnip_keyboards);

  wl_signal_subscribe(
    &catnip_backend->events.new_input,
    &new_input_listener,
    on_new_input
  );
}
