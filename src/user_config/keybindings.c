#include "keybindings.h"
#include "../user_config/events.h"
#include "lauxlib.h"
#include "user_config.h"
#include <glib.h>
#include <stdlib.h>
#include <wlr/types/wlr_keyboard.h>

static GHashTable* user_keybindings;

static void
free_user_keybinding(void* data)
{
  struct user_keybinding* user_keybinding = data;
  luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
  free(user_keybinding);
}

static gint64
get_user_keybindings_key(uint32_t modifiers, xkb_keysym_t keysym)
{
  return (keysym << WLR_MODIFIER_COUNT) | modifiers;
}

void
init_user_keybindings()
{
  user_keybindings = g_hash_table_new_full(
    g_int64_hash, g_int_equal, free, free_user_keybinding);
}

void
add_user_keybinding(uint32_t modifiers,
                    xkb_keysym_t keysym,
                    int lua_callback_ref)
{
  gint64 key = get_user_keybindings_key(modifiers, keysym);

  struct user_keybinding* user_keybinding =
    g_hash_table_lookup(user_keybindings, &key);

  if (user_keybinding != NULL) {
    luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
    user_keybinding->lua_callback_ref = lua_callback_ref;
  } else {
    gint64* persistent_key = malloc(sizeof(gint64));
    *persistent_key = key;

    user_keybinding = malloc(sizeof(struct user_keybinding));
    user_keybinding->modifiers = modifiers;
    user_keybinding->keysym = keysym;
    user_keybinding->lua_callback_ref = lua_callback_ref;

    g_hash_table_insert(user_keybindings, persistent_key, user_keybinding);
  }
}

void
remove_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = get_user_keybindings_key(modifiers, keysym);

  struct user_keybinding* user_keybinding =
    g_hash_table_lookup(user_keybindings, &key);

  if (user_keybinding != NULL) {
    g_hash_table_remove(user_keybindings, &key);
  }
}

void
clear_user_keybindings()
{
  g_hash_table_remove_all(user_keybindings);
}

bool
handle_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = get_user_keybindings_key(modifiers, keysym);

  struct user_keybinding* user_keybinding =
    g_hash_table_lookup(user_keybindings, &key);

  if (user_keybinding == NULL) {
    return false;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

  if (lua_pcall(L, 0, 0, 0) != 0) {
    g_warning("%s", lua_tostring(L, -1));
  }

  if (user_config_request_reload) {
    user_config_request_reload = false;
    call_event_listeners("reload");
    reload_user_config();
  }

  return true;
}
