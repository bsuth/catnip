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

struct catnip_key_event {
  uint32_t modifiers;
  xkb_keysym_t xkb_keysym;
  const char* xkb_name;
  enum wl_keyboard_key_state state;
  bool prevent_notify;
};

void
catnip_keyboard_create(struct wlr_input_device* device);

#endif
