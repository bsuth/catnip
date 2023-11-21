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

void
catnip_keyboard_create(struct wlr_input_device* device);

#endif
