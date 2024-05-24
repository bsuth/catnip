#ifndef CATNIP_KEY_EVENT_H
#define CATNIP_KEY_EVENT_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-server-protocol.h>
#include <xkbcommon/xkbcommon.h>

struct catnip_key_event {
  uint32_t modifiers;
  xkb_keysym_t xkb_keysym;
  const char* xkb_name;
  enum wl_keyboard_key_state state;
  bool prevent_notify;
};

#endif
