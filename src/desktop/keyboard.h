#ifndef CATNIP_DESKTOP_KEYBOARD_H
#define CATNIP_DESKTOP_KEYBOARD_H

#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

struct catnip_keyboard {
  int id;
  struct wl_list link;
  struct catnip_lua_keyboard* lua_keyboard;

  struct {
    struct wlr_keyboard* keyboard;
  } wlr;

  struct {
    char* rules;
    char* model;
    char* layout;
    char* variant;
    char* options;
  } xkb_rule_names;

  struct {
    struct wl_listener keyboard_modifiers;
    struct wl_listener keyboard_key;
    struct wl_listener keyboard_destroy;
  } listeners;

  struct {
    struct wl_event_source* reload_keymap;
  } event_sources;
};

struct catnip_keyboard*
catnip_keyboard_create(struct wlr_keyboard* wlr_keyboard);

void
catnip_keyboard_update_xkb_rule_name(
  struct catnip_keyboard* keyboard,
  char** xkb_rule_name,
  const char* value
);

#endif
