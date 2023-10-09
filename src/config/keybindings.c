#include "keybindings.h"
#include "config/config.h"
#include "config/events.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <wlr/types/wlr_keyboard.h>

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static GHashTable* config_keybindings;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static gint64
get_config_keybindings_key(uint32_t modifiers, xkb_keysym_t keysym)
{
  return (keysym << WLR_MODIFIER_COUNT) | modifiers;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
free_config_keybinding(void* data)
{
  struct config_keybinding* keybinding = data;
  luaL_unref(L, LUA_REGISTRYINDEX, keybinding->lua_callback_ref);
  free(keybinding);
}

void
init_config_keybindings()
{
  config_keybindings = g_hash_table_new_full(
    g_int64_hash,
    g_int_equal,
    free,
    free_config_keybinding
  );
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

void
add_config_keybinding(
  uint32_t modifiers,
  xkb_keysym_t keysym,
  int lua_callback_ref
)
{
  gint64 key = get_config_keybindings_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding != NULL) {
    luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
    user_keybinding->lua_callback_ref = lua_callback_ref;
  } else {
    gint64* persistent_key = malloc(sizeof(gint64));
    *persistent_key = key;

    user_keybinding = malloc(sizeof(struct config_keybinding));
    user_keybinding->modifiers = modifiers;
    user_keybinding->keysym = keysym;
    user_keybinding->lua_callback_ref = lua_callback_ref;

    g_hash_table_insert(config_keybindings, persistent_key, user_keybinding);
  }
}

void
remove_config_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = get_config_keybindings_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding != NULL) {
    g_hash_table_remove(config_keybindings, &key);
  }
}

void
clear_config_keybindings()
{
  g_hash_table_remove_all(config_keybindings);
}

bool
handle_config_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = get_config_keybindings_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding == NULL) {
    return false;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

  if (lua_pcall(L, 0, 0, 0) != 0) {
    g_warning("%s", lua_tostring(L, -1));
  }

  if (config_reload_requested) {
    config_reload_requested = false;
    reload_config();
  }

  return true;
}
