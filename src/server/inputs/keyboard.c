#include "keyboard.h"
#include "../../user_config/keybindings.h"
#include "../display.h"
#include "../seat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

struct wl_list server_keyboards;

static void
keyboard_modifiers_notify(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, modifiers_listener);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(server_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(server_seat,
                                     &keyboard->wlr_keyboard->modifiers);
}

static void
keyboard_key_notify(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, key_listener);

  struct wlr_keyboard_key_event* event = data;

  if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    uint32_t xkb_keycode = event->keycode + 8; // libinput -> xkbcommon

    xkb_keysym_t keysym =
      xkb_state_key_get_one_sym(keyboard->wlr_keyboard->xkb_state, xkb_keycode);

    if (handle_user_keybinding(modifiers, keysym)) {
      // Do not forward key events that have been handled by the compositor
      return;
    }
  }

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(server_seat, keyboard->wlr_keyboard);

  // Forward the key event to clients
  wlr_seat_keyboard_notify_key(
    server_seat, event->time_msec, event->keycode, event->state);
}

static void
keyboard_destroy_notify(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, destroy_listener);

  wl_list_remove(&keyboard->modifiers_listener.link);
  wl_list_remove(&keyboard->key_listener.link);
  wl_list_remove(&keyboard->destroy_listener.link);
  wl_list_remove(&keyboard->link);

  free(keyboard);
}

void
register_new_keyboard(struct wlr_input_device* device)
{
  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);

  struct server_keyboard* keyboard = calloc(1, sizeof(struct server_keyboard));

  keyboard->wlr_input_device = device;
  keyboard->wlr_keyboard = wlr_keyboard;

  // TODO: look into this more
  /* We need to prepare an XKB keymap and assign it to the keyboard. This
   * assumes the defaults (e.g. layout = "us"). */
  struct xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* keymap =
    xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
  wlr_keyboard_set_keymap(wlr_keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);

  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  keyboard->modifiers_listener.notify = keyboard_modifiers_notify;
  wl_signal_add(&wlr_keyboard->events.modifiers, &keyboard->modifiers_listener);
  keyboard->key_listener.notify = keyboard_key_notify;
  wl_signal_add(&wlr_keyboard->events.key, &keyboard->key_listener);
  keyboard->destroy_listener.notify = keyboard_destroy_notify;
  wl_signal_add(&device->events.destroy, &keyboard->destroy_listener);

  wl_list_insert(&server_keyboards, &keyboard->link);
}

void
init_server_keyboard()
{
  wl_list_init(&server_keyboards);
}
