#include "keyboard.h"
#include "backend.h"
#include "config.h"
#include "events/event_loop.h"
#include "keyboard/lua_keyboard.h"
#include "seat/seat.h"
#include "utils/wayland.h"
#include "utils/xkbcommon.h"
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>

struct wl_list catnip_keyboards;

static struct {
  struct wl_listener new_input;
} listeners;

static void
catnip_keyboard_modifiers(struct wl_listener* listener, void* data)
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
catnip_keyboard_key(struct wl_listener* listener, void* data)
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

    struct catnip_keyboard_key_event event = {
      .modifiers = modifiers,
      .xkb_keysym = xkb_keysym,
      .xkb_name = xkb_name,
      .state = wlr_event->state,
      .prevent_notify = false,
    };

    lua_catnip_keyboard_publish_key_event(catnip_L, keyboard, &event);

    if (!event.prevent_notify) {
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
catnip_keyboard_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.destroy);

  if (catnip_L != NULL) {
    lua_catnip_keyboard_destroy(catnip_L, keyboard);
  }

  wl_list_remove(&keyboard->link);
  wl_list_remove(&keyboard->listeners.modifiers.link);
  wl_list_remove(&keyboard->listeners.key.link);
  wl_list_remove(&keyboard->listeners.destroy.link);

  free(keyboard);
}

static void
catnip_keyboard_create(struct wl_listener* listener, void* data)
{
  struct wlr_input_device* device = data;

  if (device->type != WLR_INPUT_DEVICE_KEYBOARD) {
    return;
  }

  struct wlr_keyboard* wlr_keyboard = wlr_keyboard_from_input_device(device);
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  struct catnip_keyboard* keyboard = calloc(1, sizeof(struct catnip_keyboard));
  keyboard->wlr_keyboard = wlr_keyboard;

  keyboard->xkb_keymap_event_source = NULL;
  catnip_keyboard_reload_keymap(keyboard);

  wl_setup_listener(
    &keyboard->listeners.modifiers,
    &wlr_keyboard->events.modifiers,
    catnip_keyboard_modifiers
  );
  wl_setup_listener(
    &keyboard->listeners.key,
    &wlr_keyboard->events.key,
    catnip_keyboard_key
  );
  wl_setup_listener(
    &keyboard->listeners.destroy,
    &device->events.destroy,
    catnip_keyboard_destroy
  );

  wl_list_insert(&catnip_keyboards, &keyboard->link);

  if (catnip_L != NULL) {
    lua_catnip_keyboard_create(catnip_L, keyboard);
  }
}

void
catnip_keyboard_init()
{
  wl_list_init(&catnip_keyboards);

  wl_setup_listener(
    &listeners.new_input,
    &catnip_backend->events.new_input,
    catnip_keyboard_create
  );
}

static void
__catnip_keyboard_reload_keymap(void* data)
{
  struct catnip_keyboard* keyboard = data;

  struct xkb_context* xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_names(
    xkb_context,
    &keyboard->xkb_rule_names,
    XKB_KEYMAP_COMPILE_NO_FLAGS
  );

  wlr_keyboard_set_keymap(keyboard->wlr_keyboard, xkb_keymap);

  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);

  keyboard->xkb_keymap_event_source = NULL;
}

void
catnip_keyboard_reload_keymap(struct catnip_keyboard* keyboard)
{
  if (keyboard->xkb_keymap_event_source != NULL) {
    return;
  }

  keyboard->xkb_keymap_event_source =
    catnip_event_loop_once(__catnip_keyboard_reload_keymap, keyboard);
}