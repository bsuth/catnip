#include "keyboard.h"
#include "input/lua_key_event.h"
#include "seat.h"
#include "utils/wayland.h"
#include <stdlib.h>

static void
on_modifiers(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.modifiers);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(catnip_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(
    catnip_seat,
    &keyboard->wlr_keyboard->modifiers
  );
}

static void
on_key(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.key);

  struct wlr_keyboard_key_event* wlr_event = data;

  struct catnip_key_event* event = malloc(sizeof(struct catnip_key_event));

  event->wlr_event = wlr_event;
  event->prevent_notify = false;
  event->modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
  event->xkb_keycode = wlr_event->keycode + 8; // libinput -> xkbcommon
  event->keysym = xkb_state_key_get_one_sym(
    keyboard->wlr_keyboard->xkb_state,
    event->xkb_keycode
  );

  // TODO: do not publish event if key is just modifier
  // TODO: make keys more user friendly (provide chars when available)
  lua_catnip_publish_key_event(event);

  if (!event->prevent_notify) {
    // Wayland only allows a single keyboard per seat. Thus, we assign all
    // keyboards to the same seat, swapping them out on key events.
    wlr_seat_set_keyboard(catnip_seat, keyboard->wlr_keyboard);

    // Forward the key event to clients
    wlr_seat_keyboard_notify_key(
      catnip_seat,
      wlr_event->time_msec,
      wlr_event->keycode,
      wlr_event->state
    );
  }
}

static void
on_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.destroy);

  wl_list_remove(&keyboard->listeners.modifiers.link);
  wl_list_remove(&keyboard->listeners.key.link);
  wl_list_remove(&keyboard->listeners.destroy.link);

  free(keyboard);
}

void
catnip_keyboard_create(struct wlr_input_device* device)
{
  struct catnip_keyboard* keyboard = calloc(1, sizeof(struct catnip_keyboard));

  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);
  keyboard->wlr_keyboard = wlr_keyboard;

  // TODO: look into this more
  // We need to prepare an XKB keymap and assign it to the keyboard. This
  // assumes the defaults (e.g. layout = "us").
  struct xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* keymap =
    xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
  wlr_keyboard_set_keymap(wlr_keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);

  wl_setup_listener(
    &keyboard->listeners.modifiers,
    &wlr_keyboard->events.modifiers,
    on_modifiers
  );
  wl_setup_listener(
    &keyboard->listeners.key,
    &wlr_keyboard->events.key,
    on_key
  );
  wl_setup_listener(
    &keyboard->listeners.destroy,
    &device->events.destroy,
    on_destroy
  );
}
