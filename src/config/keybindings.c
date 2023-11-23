#include "keybindings.h"
#include "config/config.h"
#include "config/events.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <wlr/types/wlr_keyboard.h>

static GHashTable* config_keybindings = NULL;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static gint64
config_keybindings_get_key(uint32_t modifiers, xkb_keysym_t keysym)
{
  return (keysym << WLR_MODIFIER_COUNT) | modifiers;
}

// -----------------------------------------------------------------------------
// Keybindings
// -----------------------------------------------------------------------------

void
config_keybindings_bind(
  uint32_t modifiers,
  xkb_keysym_t keysym,
  int lua_callback_ref
)
{
  gint64 key = config_keybindings_get_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding != NULL) {
    luaL_unref(catnip_L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);
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
config_keybindings_unbind(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = config_keybindings_get_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding != NULL) {
    g_hash_table_remove(config_keybindings, &key);
  }
}

void
config_keybindings_clear()
{
  g_hash_table_remove_all(config_keybindings);
}

bool
config_keybinding_handle(uint32_t modifiers, xkb_keysym_t keysym)
{
  gint64 key = config_keybindings_get_key(modifiers, keysym);

  struct config_keybinding* user_keybinding =
    g_hash_table_lookup(config_keybindings, &key);

  if (user_keybinding == NULL) {
    return false;
  }

  lua_rawgeti(catnip_L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

  if (lua_pcall(catnip_L, 0, 0, 0) != 0) {
    log_error("%s", lua_tostring(catnip_L, -1));
  }

  return true;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
free_config_keybinding(void* data)
{
  struct config_keybinding* keybinding = data;
  luaL_unref(catnip_L, LUA_REGISTRYINDEX, keybinding->lua_callback_ref);
  free(keybinding);
}

void
config_keybindings_init()
{
  config_keybindings = g_hash_table_new_full(
    g_int64_hash,
    g_int_equal,
    free,
    free_config_keybinding
  );
}
