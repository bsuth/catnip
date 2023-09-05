#include "keybindings.h"
#include "lua.h"
#include "user_config.h"
#include <glib.h>

static GList* user_keybindings_list;

void
add_user_keybinding(uint32_t modifiers,
                    xkb_keysym_t keysym,
                    int lua_callback_ref)
{
  struct user_keybinding* new_keybinding =
    malloc(sizeof(struct user_keybinding));

  new_keybinding->modifiers = modifiers;
  new_keybinding->keysym = keysym;
  new_keybinding->lua_callback_ref = lua_callback_ref;

  user_keybindings_list = g_list_append(user_keybindings_list, new_keybinding);
}

bool
handle_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  for (GList* item = user_keybindings_list; item; item = item->next) {
    struct user_keybinding* user_keybinding = item->data;

    if (user_keybinding->modifiers == modifiers &&
        user_keybinding->keysym == keysym) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
      lua_call(L, 0, 0);
    }
  }

  return false;
}
