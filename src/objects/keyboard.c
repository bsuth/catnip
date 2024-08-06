#include "keyboard.h"
#include "event_loop.h"
#include "extensions/wayland.h"
#include "id.h"
#include "objects/key_event.h"
#include "seat.h"
#include <stdlib.h>
#include <string.h>

struct _catnip_keyboard {
  int id;
  struct wl_list link;

  struct wlr_keyboard* wlr_keyboard;

  struct {
    struct wl_listener wlr_keyboard_modifiers;
    struct wl_listener wlr_keyboard_key;
    struct wl_listener wlr_keyboard_destroy;
  } listeners;

  struct {
    struct wl_signal destroy;
    struct wl_signal key;
  } signals;

  struct {
    struct wl_event_source* reload_keymap;
  } event_sources;

  struct {
    char* rules;
    char* model;
    char* layout;
    char* variant;
    char* options;
  } xkb_rule_names;
};

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
reload_keymap(void* data)
{
  struct _catnip_keyboard* keyboard = data;

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

  wlr_keyboard_set_keymap(keyboard->wlr_keyboard, xkb_keymap);

  xkb_keymap_unref(xkb_keymap);
  xkb_context_unref(xkb_context);

  keyboard->event_sources.reload_keymap = NULL;
}

void
queue_reload_keymap(struct _catnip_keyboard* keyboard)
{
  if (keyboard->event_sources.reload_keymap != NULL) {
    return;
  }

  keyboard->event_sources.reload_keymap =
    wl_event_loop_add_idle(catnip_event_loop, reload_keymap, keyboard);
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static void
on_wlr_keyboard_modifiers(struct wl_listener* listener, void* data)
{
  struct _catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.wlr_keyboard_modifiers);

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(catnip_seat, keyboard->wlr_keyboard);

  wlr_seat_keyboard_notify_modifiers(
    catnip_seat,
    &keyboard->wlr_keyboard->modifiers
  );
}

static void
on_wlr_keyboard_key(struct wl_listener* listener, void* data)
{
  struct wlr_keyboard_key_event* wlr_key_event = data;

  struct _catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.wlr_keyboard_key);

  catnip_key_event_t* key_event =
    catnip_key_event_create(keyboard->wlr_keyboard, wlr_key_event);

  if (catnip_key_event_check_keybindings(key_event)) {
    return;
  }

  wl_signal_emit(&keyboard->signals.key, key_event);

  if (catnip_key_event_get_prevent_notify(key_event)) {
    return;
  }

  // Wayland only allows a single keyboard per seat. Thus, we assign all
  // keyboards to the same seat, swapping them out on key events.
  wlr_seat_set_keyboard(catnip_seat, keyboard->wlr_keyboard);

  // Forward the key event to clients
  wlr_seat_keyboard_notify_key(
    catnip_seat,
    wlr_key_event->time_msec,
    wlr_key_event->keycode,
    wlr_key_event->state
  );
}

static void
on_wlr_keyboard_destroy(struct wl_listener* listener, void* data)
{
  struct _catnip_keyboard* keyboard =
    wl_container_of(listener, keyboard, listeners.wlr_keyboard_destroy);

  catnip_keyboard_destroy(keyboard);
}

struct _catnip_keyboard*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard)
{
  struct _catnip_keyboard* keyboard = malloc(sizeof(struct _catnip_keyboard));
  keyboard->id = generate_catnip_id();
  keyboard->wlr_keyboard = wlr_keyboard;

  keyboard->xkb_rule_names.rules = NULL;
  keyboard->xkb_rule_names.model = NULL;
  keyboard->xkb_rule_names.layout = NULL;
  keyboard->xkb_rule_names.variant = NULL;
  keyboard->xkb_rule_names.options = NULL;

  keyboard->event_sources.reload_keymap = NULL;
  queue_reload_keymap(keyboard);

  wl_signal_init(&keyboard->signals.key);
  wl_signal_init(&keyboard->signals.destroy);

  wl_signal_subscribe(
    &wlr_keyboard->events.modifiers,
    &keyboard->listeners.wlr_keyboard_modifiers,
    on_wlr_keyboard_modifiers
  );
  wl_signal_subscribe(
    &wlr_keyboard->events.key,
    &keyboard->listeners.wlr_keyboard_key,
    on_wlr_keyboard_key
  );
  wl_signal_subscribe(
    &wlr_keyboard->base.events.destroy,
    &keyboard->listeners.wlr_keyboard_destroy,
    on_wlr_keyboard_destroy
  );

  return keyboard;
}

void
catnip_keyboard_destroy(struct _catnip_keyboard* keyboard)
{
  wl_signal_emit(&keyboard->signals.destroy, keyboard);

  wl_list_remove(&keyboard->link);

  wl_list_remove(&keyboard->listeners.wlr_keyboard_modifiers.link);
  wl_list_remove(&keyboard->listeners.wlr_keyboard_key.link);
  wl_list_remove(&keyboard->listeners.wlr_keyboard_destroy.link);

  wl_list_remove(&keyboard->signals.destroy.listener_list);
  wl_list_remove(&keyboard->signals.key.listener_list);

  if (keyboard->event_sources.reload_keymap != NULL) {
    wl_event_source_remove(keyboard->event_sources.reload_keymap);
  }

  free(keyboard);
}

void
catnip_keyboard_link(struct _catnip_keyboard* keyboard, struct wl_list* list)
{
  wl_list_insert(list, &keyboard->link);
}

void
catnip_keyboard_subscribe(
  struct _catnip_keyboard* keyboard,
  enum catnip_keyboard_event event,
  struct wl_listener* listener,
  wl_notify_func_t callback
)
{
  switch (event) {
    case CATNIP_KEYBOARD_EVENT_DESTROY:
      wl_signal_subscribe(&keyboard->signals.destroy, listener, callback);
      break;
    case CATNIP_KEYBOARD_EVENT_KEY:
      wl_signal_subscribe(&keyboard->signals.key, listener, callback);
      break;
  }
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

const int
catnip_keyboard_get_id(struct _catnip_keyboard* keyboard)
{
  return keyboard->id;
}

const char*
catnip_keyboard_get_name(struct _catnip_keyboard* keyboard)
{
  return keyboard->wlr_keyboard->base.name;
}

const char*
catnip_keyboard_get_xkb_rules(struct _catnip_keyboard* keyboard)
{
  return keyboard->xkb_rule_names.rules;
}

const char*
catnip_keyboard_get_xkb_model(struct _catnip_keyboard* keyboard)
{
  return keyboard->xkb_rule_names.model;
}

const char*
catnip_keyboard_get_xkb_layout(struct _catnip_keyboard* keyboard)
{
  return keyboard->xkb_rule_names.layout;
}

const char*
catnip_keyboard_get_xkb_variant(struct _catnip_keyboard* keyboard)
{
  return keyboard->xkb_rule_names.variant;
}

const char*
catnip_keyboard_get_xkb_options(struct _catnip_keyboard* keyboard)
{
  return keyboard->xkb_rule_names.options;
}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------

void
catnip_keyboard_set_xkb_rules(
  struct _catnip_keyboard* keyboard,
  const char* new_xkb_rules
)
{
  if (keyboard->xkb_rule_names.rules != NULL) {
    free(keyboard->xkb_rule_names.rules);
  }

  keyboard->xkb_rule_names.rules =
    new_xkb_rules == NULL ? NULL : strdup(new_xkb_rules);

  queue_reload_keymap(keyboard);
}

void
catnip_keyboard_set_xkb_model(
  struct _catnip_keyboard* keyboard,
  const char* new_xkb_model
)
{
  if (keyboard->xkb_rule_names.model != NULL) {
    free(keyboard->xkb_rule_names.model);
  }

  keyboard->xkb_rule_names.model =
    new_xkb_model == NULL ? NULL : strdup(new_xkb_model);

  queue_reload_keymap(keyboard);
}

void
catnip_keyboard_set_xkb_layout(
  struct _catnip_keyboard* keyboard,
  const char* new_xkb_layout
)
{
  if (keyboard->xkb_rule_names.layout != NULL) {
    free(keyboard->xkb_rule_names.layout);
  }

  keyboard->xkb_rule_names.layout =
    new_xkb_layout == NULL ? NULL : strdup(new_xkb_layout);

  queue_reload_keymap(keyboard);
}

void
catnip_keyboard_set_xkb_variant(
  struct _catnip_keyboard* keyboard,
  const char* new_xkb_variant
)
{
  if (keyboard->xkb_rule_names.variant != NULL) {
    free(keyboard->xkb_rule_names.variant);
  }

  keyboard->xkb_rule_names.variant =
    new_xkb_variant == NULL ? NULL : strdup(new_xkb_variant);

  queue_reload_keymap(keyboard);
}

void
catnip_keyboard_set_xkb_options(
  struct _catnip_keyboard* keyboard,
  const char* new_xkb_options
)
{
  if (keyboard->xkb_rule_names.options != NULL) {
    free(keyboard->xkb_rule_names.options);
  }

  keyboard->xkb_rule_names.options =
    new_xkb_options == NULL ? NULL : strdup(new_xkb_options);

  queue_reload_keymap(keyboard);
}
