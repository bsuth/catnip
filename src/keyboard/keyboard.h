#ifndef CATNIP_KEYBOARD_H
#define CATNIP_KEYBOARD_H

#include "lua_resource.h"
#include <wlr/types/wlr_seat.h>

struct catnip_keyboard {
  struct wl_list link;
  struct catnip_lua_resource* lua_resource;

  struct wlr_keyboard* wlr_keyboard;

  struct xkb_rule_names xkb_rule_names;
  struct wl_event_source* xkb_keymap_event_source;

  struct {
    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
  } listeners;
};

struct catnip_key_event {
  uint32_t modifiers;
  xkb_keysym_t xkb_keysym;
  const char* xkb_name;
  enum wl_keyboard_key_state state;
  bool prevent_notify;
};

extern struct wl_list catnip_keyboards;

void
catnip_keyboard_init();

#endif
