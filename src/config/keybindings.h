#ifndef CATNIP_CONFIG_KEYBINDINGS_H
#define CATNIP_CONFIG_KEYBINDINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>

struct config_keybinding {
  uint32_t modifiers;
  xkb_keysym_t keysym;
  int lua_callback_ref;
};

void
config_keybindings_bind(
  uint32_t modifiers,
  xkb_keysym_t keysym,
  int lua_callback_ref
);

void
config_keybindings_unbind(uint32_t modifiers, xkb_keysym_t keysym);

void
config_keybindings_clear();

bool
config_keybinding_handle(uint32_t modifiers, xkb_keysym_t keysym);

void
config_keybindings_init();

#endif
