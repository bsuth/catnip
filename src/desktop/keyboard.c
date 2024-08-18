#include "keyboard.h"
#include "compositor/event_loop.h"
#include "compositor/seat.h"
#include "config.h"
#include "desktop/key_event.h"
#include "desktop/lua_key_event.h"
#include "desktop/lua_keyboard.h"
#include "extensions/wayland.h"
#include "id.h"
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
catnip_keyboard_reload_keymap(void* data)
{
  struct catnip_keyboard* keyboard = data;

  struct xkb_rule_names xkb_rule_names = {
    .rules = keyboard->xkb_rule_names.rules,
    .model = keyboard->xkb_rule_names.model,
    .layout = keyboard->xkb_rule_names.layout,
    .variant = keyboard->xkb_rule_names.variant,
    .options = keyboard->xkb_rule_names.options,
  };

  struct xkb_context* xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_names(
    xkb_context,
    &xkb_rule_names,
    XKB_KEYMAP_COMPILE_NO_FLAGS
  );

  wlr_keyboard_set_keymap(keyboard->wlr.keyboard, xkb_keymap);

  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);

  keyboard->event_sources.reload_keymap = NULL;
}

// -----------------------------------------------------------------------------
// Create
// -----------------------------------------------------------------------------

static void
on_keyboard_modifiers(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.keyboard_modifiers);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(catnip_seat, keyboard->wlr.keyboard);

  wlr_seat_keyboard_notify_modifiers(
    catnip_seat,
    &keyboard->wlr.keyboard->modifiers
  );
}

static void
on_keyboard_key(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.keyboard_key);

  struct wlr_keyboard_key_event* wlr_event = data;

  struct catnip_key_event event = {
    .modifiers = wlr_keyboard_get_modifiers(keyboard->wlr.keyboard),
    .xkb_keysym = xkb_state_key_get_one_sym(
      keyboard->wlr.keyboard->xkb_state,
      wlr_event->keycode + 8 // libinput -> xkbcommon
    ),
    .state = wlr_event->state,
    .prevent_notify = false,
  };

  if (catnip_key_event_check_keybindings(&event)) {
    return;
  }

  catnip_lua_key_event_publish(catnip_L, keyboard, &event);

  if (!event.prevent_notify) {
    // Wayland only allows a single keyboard per seat. Thus, we assign all
    // keyboards to the same seat, swapping them out on key events.
    wlr_seat_set_keyboard(catnip_seat, keyboard->wlr.keyboard);

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
on_keyboard_destroy(struct wl_listener* listener, void* data)
{
  struct catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.keyboard_destroy);

  catnip_lua_keyboard_destroy(catnip_L, keyboard->lua_keyboard);

  wl_list_remove(&keyboard->link);
  wl_list_remove(&keyboard->listeners.keyboard_modifiers.link);
  wl_list_remove(&keyboard->listeners.keyboard_key.link);
  wl_list_remove(&keyboard->listeners.keyboard_destroy.link);

  if (keyboard->event_sources.reload_keymap != NULL) {
    wl_event_source_remove(keyboard->event_sources.reload_keymap);
  }

  free(keyboard);
}

struct catnip_keyboard*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard)
{
  wlr_keyboard_set_repeat_info(wlr_keyboard, 25, 600);

  struct catnip_keyboard* keyboard = calloc(1, sizeof(struct catnip_keyboard));
  keyboard->id = generate_catnip_id();
  keyboard->wlr.keyboard = wlr_keyboard;

  keyboard->xkb_rule_names.rules = NULL;
  keyboard->xkb_rule_names.model = NULL;
  keyboard->xkb_rule_names.layout = NULL;
  keyboard->xkb_rule_names.variant = NULL;
  keyboard->xkb_rule_names.options = NULL;

  keyboard->event_sources.reload_keymap = NULL;
  catnip_keyboard_reload_keymap(keyboard);

  wl_signal_subscribe(
    &wlr_keyboard->events.modifiers,
    &keyboard->listeners.keyboard_modifiers,
    on_keyboard_modifiers
  );
  wl_signal_subscribe(
    &wlr_keyboard->events.key,
    &keyboard->listeners.keyboard_key,
    on_keyboard_key
  );
  wl_signal_subscribe(
    &wlr_keyboard->base.events.destroy,
    &keyboard->listeners.keyboard_destroy,
    on_keyboard_destroy
  );

  catnip_lua_keyboard_create(catnip_L, keyboard);

  return keyboard;
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_keyboard_update_xkb_rule_name(
  struct catnip_keyboard* keyboard,
  char** xkb_rule_name,
  const char* value
)
{
  if (*xkb_rule_name != NULL) {
    free(*xkb_rule_name);
  }

  *xkb_rule_name = value == NULL ? NULL : strdup(value);

  if (keyboard->event_sources.reload_keymap != NULL) {
    keyboard->event_sources.reload_keymap = wl_event_loop_add_idle(
      catnip_event_loop,
      catnip_keyboard_reload_keymap,
      keyboard
    );
  }
}
