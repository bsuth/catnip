#include "keyboard.h"
#include "input/lua_key_event.h"
#include "seat.h"
#include "utils/wayland.h"
#include "utils/xkbcommon.h"
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>

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

  uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);

  const xkb_keysym_t* xkb_keysyms = NULL;
  int xkb_keysyms_len = xkb_state_key_get_syms(
    keyboard->wlr_keyboard->xkb_state,
    wlr_event->keycode + 8, // libinput -> xkbcommon
    &xkb_keysyms
  );

  for (int i = 0; i < xkb_keysyms_len; ++i) {
    xkb_keysym_t xkb_keysym = xkb_keysyms[i];

    if (xkb_is_modifier(xkb_keysym)) {
      continue; // do not publish key events for modifiers.
    }

    char xkb_name[64];
    xkb_keysym_get_name(xkb_keysym, xkb_name, 64);

    struct catnip_key_event key_event = {
      .modifiers = modifiers,
      .xkb_keysym = xkb_keysym,
      .xkb_name = xkb_name,
      .state = wlr_event->state,
      .prevent_notify = false,
    };

    lua_catnip_publish_key_event(&key_event);

    if (!key_event.prevent_notify) {
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
  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  // TODO: allow users to configure the keymap?
  struct xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* keymap =
    xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
  wlr_keyboard_set_keymap(wlr_keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);

  struct catnip_keyboard* keyboard = calloc(1, sizeof(struct catnip_keyboard));
  keyboard->wlr_keyboard = wlr_keyboard;

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
