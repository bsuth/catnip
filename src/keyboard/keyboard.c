#include "keyboard.h"
#include "config.h"
#include "event_loop.h"
#include "id.h"
#include "keyboard/key_event.h"
#include "keyboard/lua_key_event.h"
#include "keyboard/lua_keyboard.h"
#include "seat.h"
#include "utils/wayland.h"
#include <stdlib.h>

static void
on_keyboard_modifiers(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, modifiers_listener);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(catnip_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(
    catnip_seat,
    &keyboard->wlr_keyboard->modifiers
  );
}

static void
on_keyboard_key(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, key_listener);

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

    char xkb_name[64];
    xkb_keysym_get_name(xkb_keysym, xkb_name, 64);

    struct catnip_key_event event = {
      .modifiers = modifiers,
      .xkb_keysym = xkb_keysym,
      .xkb_name = xkb_name,
      .state = wlr_event->state,
      .prevent_notify = false,
    };

    lua_catnip_publish_key_event(catnip_L, keyboard, &event);

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
on_keyboard_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, destroy_listener);

  lua_catnip_keyboard_destroy(catnip_L, keyboard->lua_resource);

  wl_list_remove(&keyboard->link);
  wl_list_remove(&keyboard->modifiers_listener.link);
  wl_list_remove(&keyboard->key_listener.link);
  wl_list_remove(&keyboard->destroy_listener.link);

  free(keyboard);
}

struct catnip_keyboard*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard)
{
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  struct catnip_keyboard* keyboard = calloc(1, sizeof(struct catnip_keyboard));
  keyboard->id = generate_catnip_id();
  keyboard->wlr_keyboard = wlr_keyboard;

  keyboard->xkb_rules = NULL;
  keyboard->xkb_model = NULL;
  keyboard->xkb_layout = NULL;
  keyboard->xkb_variant = NULL;
  keyboard->xkb_options = NULL;

  keyboard->reload_keymap_event_source = NULL;
  catnip_keyboard_reload_keymap(keyboard);

  wl_setup_listener(
    &keyboard->modifiers_listener,
    &wlr_keyboard->events.modifiers,
    on_keyboard_modifiers
  );
  wl_setup_listener(
    &keyboard->key_listener,
    &wlr_keyboard->events.key,
    on_keyboard_key
  );
  wl_setup_listener(
    &keyboard->destroy_listener,
    &wlr_keyboard->base.events.destroy,
    on_keyboard_destroy
  );

  lua_catnip_keyboard_create(catnip_L, keyboard);

  return keyboard;
}

static void
__catnip_keyboard_reload_keymap(void* data)
{
  struct catnip_keyboard* keyboard = data;

  struct xkb_rule_names xkb_rule_names = {
    .rules = keyboard->xkb_rules,
    .model = keyboard->xkb_model,
    .layout = keyboard->xkb_layout,
    .variant = keyboard->xkb_variant,
    .options = keyboard->xkb_options,
  };

  struct xkb_context* xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_names(
    xkb_context,
    &xkb_rule_names,
    XKB_KEYMAP_COMPILE_NO_FLAGS
  );

  wlr_keyboard_set_keymap(keyboard->wlr_keyboard, xkb_keymap);

  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);

  keyboard->reload_keymap_event_source = NULL;
}

void
catnip_keyboard_reload_keymap(struct catnip_keyboard* keyboard)
{
  if (keyboard->reload_keymap_event_source != NULL) {
    return;
  }

  keyboard->reload_keymap_event_source = wl_event_loop_add_idle(
    catnip_event_loop,
    __catnip_keyboard_reload_keymap,
    keyboard
  );
}
