#include "init.h"
#include "backend.h"
#include "keyboard/keyboard.h"
#include "keyboard/keyboards.h"
#include "utils/wayland.h"

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

  wl_setup_listener(
    &new_input_listener,
    &catnip_backend->events.new_input,
    on_new_input
  );
}