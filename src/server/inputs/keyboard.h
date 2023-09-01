#ifndef BWC_SERVER_INPUTS_KEYBOARD_H
#define BWC_SERVER_INPUTS_KEYBOARD_H

#include <wlr/types/wlr_seat.h>

struct bwc_keyboard
{
  struct wl_list link;
  struct wlr_keyboard* wlr_keyboard;

  struct wl_listener modifiers_listener;
  struct wl_listener key_listener;
  struct wl_listener destroy_listener;
};

extern struct wl_list bwc_keyboards;

void
register_new_keyboard(struct wlr_input_device* device);

void
bwc_keyboard_init();

#endif
