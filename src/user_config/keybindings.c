#include "keybindings.h"
#include "lauxlib.h"
#include "user_config.h"
#include <glib.h>

// The maximum number of allowed keybindings. This is the size of the
// `user_keybindings` array, which is just a normal array due to speed and
// simplicity. If for some reason users are setting more than 128 keybindings,
// this will need to be incremented.
#define MAX_KEYBINDINGS 128

static struct user_keybinding user_keybindings[MAX_KEYBINDINGS];
static int user_keybindings_len = 0;

void
add_user_keybinding(uint32_t modifiers,
                    xkb_keysym_t keysym,
                    int lua_callback_ref)
{

  for (int i = 0; i < user_keybindings_len; ++i) {
    struct user_keybinding* user_keybinding = &user_keybindings[i];

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

  if (user_keybindings_len == MAX_KEYBINDINGS) {
    g_warning("cannot add more than %d keybindings", MAX_KEYBINDINGS);
  }

  struct user_keybinding* user_keybinding =
    &user_keybindings[user_keybindings_len];

  user_keybinding->modifiers = modifiers;
  user_keybinding->keysym = keysym;
  user_keybinding->lua_callback_ref = lua_callback_ref;

  ++user_keybindings_len;
}

void
remove_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  for (int i = 0; i < user_keybindings_len; ++i) {
    struct user_keybinding* user_keybinding = &user_keybindings[i];

    const bool is_matching_keybinding =
      user_keybinding->modifiers == modifiers &&
      user_keybinding->keysym == keysym;

    if (is_matching_keybinding) {
      luaL_unref(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

      // Since order does not matter, we simply copy the last element to the
      // deleted element's index and update the length.
      struct user_keybinding* last_user_keybinding =
        &user_keybindings[user_keybindings_len - 1];

      user_keybinding->modifiers = last_user_keybinding->modifiers;
      user_keybinding->keysym = last_user_keybinding->keysym;
      user_keybinding->lua_callback_ref =
        last_user_keybinding->lua_callback_ref;

      --user_keybindings_len;

      return;
    }
  }
}

void
clear_user_keybindings()
{
  for (int i = 0; i < user_keybindings_len; ++i) {
    luaL_unref(L, LUA_REGISTRYINDEX, user_keybindings[i].lua_callback_ref);
  }

  user_keybindings_len = 0;
}

bool
handle_user_keybinding(uint32_t modifiers, xkb_keysym_t keysym)
{
  for (int i = 0; i < user_keybindings_len; ++i) {
    struct user_keybinding* user_keybinding = &user_keybindings[i];

    const bool is_matching_keybinding =
      user_keybinding->modifiers == modifiers &&
      user_keybinding->keysym == keysym;

    if (is_matching_keybinding) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, user_keybinding->lua_callback_ref);

      if (lua_pcall(L, 0, 0, 0) != 0) {
        g_warning("%s", lua_tostring(L, -1));
      }

      if (user_config_request_reload) {
        user_config_request_reload = false;
        reload_user_config();
      }

      return true;
    }
  }

  return false;
}
