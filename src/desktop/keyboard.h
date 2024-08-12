#ifndef CATNIP_DESKTOP_KEYBOARD_H
#define CATNIP_DESKTOP_KEYBOARD_H

#include "lua_resource.h"
#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

struct catnip_keyboard {
  int id;

  struct wl_list link;
  struct catnip_lua_resource* lua_resource;

  struct wlr_keyboard* wlr_keyboard;

  char* xkb_rules;
  char* xkb_model;
  char* xkb_layout;
  char* xkb_variant;
  char* xkb_options;

  struct wl_listener modifiers_listener;
  struct wl_listener key_listener;
  struct wl_listener destroy_listener;

  struct wl_event_source* reload_keymap_event_source;
};

struct catnip_keyboard*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard);

void
catnip_keyboard_reload_keymap(struct catnip_keyboard* keyboard);

#endif
