#ifndef CATNIP_KEYBOARD_H
#define CATNIP_KEYBOARD_H

#include <wlr/types/wlr_seat.h>

struct catnip_keyboard {
  struct wl_list link;

  struct wlr_input_device* wlr_input_device;
  struct wlr_keyboard* wlr_keyboard;

  struct {
    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
  } listeners;
};

extern struct wl_list catnip_keyboards;

void
catnip_keyboard_create(struct wlr_input_device* device);

void
catnip_keyboard_init();

#endif
