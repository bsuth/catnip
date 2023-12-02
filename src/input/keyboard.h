#ifndef CATNIP_KEYBOARD_H
#define CATNIP_KEYBOARD_H

#include <wlr/types/wlr_seat.h>

struct catnip_keyboard {
  struct wlr_keyboard* wlr_keyboard;

  struct {
    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
  } listeners;
};

struct catnip_modifier_event {
  bool prevent_notify;
  uint32_t modifiers;
  uint32_t xkb_keycode;
  xkb_keysym_t keysym;
};

struct catnip_key_event {
  struct wlr_keyboard_key_event* wlr_event;
  bool prevent_notify;
  uint32_t modifiers;
  uint32_t xkb_keycode;
  xkb_keysym_t keysym;
};

void
catnip_keyboard_create(struct wlr_input_device* device);

#endif
