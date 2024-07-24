#ifndef CATNIP_KEY_EVENT_H
#define CATNIP_KEY_EVENT_H

#include <stdbool.h>
#include <stdint.h>
#include <wayland-server-protocol.h>
#include <xkbcommon/xkbcommon.h>

// TODO: Update this to 5!
//
// Originally, the unicode standard in [RFC 2279] (1998) specifies a maximum of
// 6 bytes per code point, hence why `XKB_KEYSYM_UTF8_MAX_SIZE` needs to be 7
// (6 code point bytes + 1 null terminating character).
//
// In [RFC 3629] (2003), the unicode standard was updated to specify a maximum
// of 4 bytes per code point, allowing us to update `XKB_KEYSYM_UTF8_MAX_SIZE`
// to 5 (4 code point bytes + 1 null terminating character).
//
// However, at the time of writing, xkbcommon was _just_ updated (2 days ago)
// to allow this new buffer size. Since this update is not available on most
// distributions yet, we stick to the legacy value for now, but should update it
// later on once the new limit is accepted on most distributions.
//
// https://github.com/xkbcommon/libxkbcommon/commit/addf73c5bb76975573ff9e7d359ef4f380594ce9
#define XKB_KEYSYM_UTF8_MAX_SIZE 7

struct catnip_key_event {
  uint32_t modifiers;
  xkb_keysym_t xkb_keysym;
  char xkb_name[64];
  char utf8[XKB_KEYSYM_UTF8_MAX_SIZE];
  enum wl_keyboard_key_state state;
  bool prevent_notify;
};

#endif
