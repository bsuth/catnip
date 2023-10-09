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
init_config_keybindings();

void
add_config_keybinding(
  uint32_t modifiers,
  xkb_keysym_t keysym,
  int lua_callback_ref
);

void
remove_config_keybinding(uint32_t modifiers, xkb_keysym_t keysym);

void
clear_config_keybindings();

bool
handle_config_keybinding(uint32_t modifiers, xkb_keysym_t keysym);

#endif
