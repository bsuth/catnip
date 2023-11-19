#include "keyboard.h"
#include "config/keybindings.h"
#include "server/display.h"
#include "server/seat.h"
#include "utils/wayland.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

struct wl_list server_keyboards;

// -----------------------------------------------------------------------------
// Create
// -----------------------------------------------------------------------------

static void
on_keyboard_modifiers(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.modifiers);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(server_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(
    server_seat,
    &keyboard->wlr_keyboard->modifiers
  );
}

static void
on_keyboard_key(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.key);

  struct wlr_keyboard_key_event* event = data;

  if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
    uint32_t xkb_keycode = event->keycode + 8; // libinput -> xkbcommon

    xkb_keysym_t keysym =
      xkb_state_key_get_one_sym(keyboard->wlr_keyboard->xkb_state, xkb_keycode);

    if (config_keybinding_handle(modifiers, keysym)) {
      // Do not forward key events that have been handled by the compositor
      return;
    }
  }

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(server_seat, keyboard->wlr_keyboard);

  // Forward the key event to clients
  wlr_seat_keyboard_notify_key(
    server_seat,
    event->time_msec,
    event->keycode,
    event->state
  );
}

static void
on_keyboard_destroy(struct wl_listener* listener, void* data)
{
  struct server_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.destroy);

  wl_list_remove(&keyboard->listeners.modifiers.link);
  wl_list_remove(&keyboard->listeners.key.link);
  wl_list_remove(&keyboard->listeners.destroy.link);
  wl_list_remove(&keyboard->link);

  free(keyboard);
}

void
server_keyboard_create(struct wlr_input_device* device)
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

  wl_setup_listener(
    &keyboard->listeners.modifiers,
    &wlr_keyboard->events.modifiers,
    on_keyboard_modifiers
  );
  wl_setup_listener(
    &keyboard->listeners.key,
    &wlr_keyboard->events.key,
    on_keyboard_key
  );
  wl_setup_listener(
    &keyboard->listeners.destroy,
    &device->events.destroy,
    on_keyboard_destroy
  );

  wl_list_insert(&server_keyboards, &keyboard->link);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
server_keyboard_init()
{
  wl_list_init(&server_keyboards);
}