#include "keybindings.h"
#include "lauxlib.h"
#include "user_config.h"
#include <glib.h>

static GList* user_keybindings_list;

static void
destroy_user_keybinding(gpointer data)
{
  struct user_keybinding* user_keybinding = data;
  luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
  free(user_keybinding);
}

void
add_user_keybinding(uint32_t modifiers,
                    xkb_keysym_t keysym,
                    int lua_callback_ref)
{
  for (GList* item = user_keybindings_list; item; item = item->next) {
    struct user_keybinding* user_keybinding = item->data;

    const bool is_matching_keybinding =
      user_keybinding->modifiers == modifiers &&
      user_keybinding->keysym == keysym;

    if (is_matching_keybinding) {
      if (user_keybinding->lua_callback_ref != lua_callback_ref) {
        luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
        user_keybinding->lua_callback_ref = lua_callback_ref;
      }
      return;
    }
  }

  struct user_keybinding* user_keybinding =
    malloc(sizeof(struct user_keybinding));

  user_keybinding->modifiers = modifiers;
  user_keybinding->keysym = keysym;
  user_keybinding->lua_callback_ref = lua_callback_ref;

  // Use `g_list_prepend` over `g_list_append` since order doesn't matter and
  // prepend is faster.
  user_keybindings_list =
    g_list_prepend(user_keybindings_list, user_keybinding);
}

void
remove_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  for (GList* item = user_keybindings_list; item; item = item->next) {
    struct user_keybinding* user_keybinding = item->data;

    const bool is_matching_keybinding =
      user_keybinding->modifiers == modifiers &&
      user_keybinding->keysym == keysym;

    if (is_matching_keybinding) {
      user_keybindings_list = g_list_remove_link(user_keybindings_list, item);
      g_list_free_full(item, destroy_user_keybinding);
      return;
    }
  }
}

void
clear_user_keybindings()
{
  g_list_free_full(g_steal_pointer(&user_keybindings_list),
                   destroy_user_keybinding);
}

bool
handle_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  for (GList* item = user_keybindings_list; item; item = item->next) {
    struct user_keybinding* user_keybinding = item->data;

    const bool is_matching_keybinding =
      user_keybinding->modifiers == modifiers &&
      user_keybinding->keysym == keysym;

    if (is_matching_keybinding) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

      if (lua_pcall(L, 0, 0, 0) != 0) {
        g_warning("%s", lua_tostring(L, -1));
      }

      return true;
    }
  }

  return false;
}
