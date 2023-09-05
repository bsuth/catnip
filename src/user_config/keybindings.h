#ifndef USER_CONFIG_KEYBINDINGS_H
#define USER_CONFIG_KEYBINDINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>

struct user_keybinding
{
  uint32_t modifiers;
  xkb_keysym_t keysym;
  int lua_callback_ref;
};

void
add_user_keybinding(uint32_t modifiers,
                    xkb_keysym_t keysym,
                    int lua_callback_ref);

bool
handle_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym);

#endif
