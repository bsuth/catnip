#include "keybindings.h"
#include "api/api.h"
#include "config/keybindings.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <glib.h>
#include <lauxlib.h>
#include <lua.h>
#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static uint32_t
translate_lua_modifier(const char* s)
{
  if (g_str_equal(s, "Shift")) {
    return WLR_MODIFIER_SHIFT;
  } else if (g_str_equal(s, "Control")) {
    return WLR_MODIFIER_CTRL;
  } else if (g_str_equal(s, "Mod1") || g_str_equal(s, "Alt")) {
    return WLR_MODIFIER_ALT;
  } else if (g_str_equal(s, "Mod2")) {
    return WLR_MODIFIER_MOD2;
  } else if (g_str_equal(s, "Mod3")) {
    return WLR_MODIFIER_MOD3;
  } else if (g_str_equal(s, "Mod4") || g_str_equal(s, "Super") || g_str_equal(s, "Meta")) {
    return WLR_MODIFIER_LOGO;
  } else if (g_str_equal(s, "Mod5")) {
    return WLR_MODIFIER_MOD5;
  } else {
    return 0;
  }
}

static uint32_t
translate_lua_key(const char* s)
{
  xkb_keysym_t keysym = xkb_keysym_from_name(s, XKB_KEYSYM_NO_FLAGS);

  if (keysym == XKB_KEY_NoSymbol && strlen(s) == 1) {
    // Allow bindings for printable characters by providing the character
    // itself instead of the name.
    const int keycode = (int) s[0];
    if (31 < keycode && keycode < 127) {
      keysym = keycode;
    }
  }

  return keysym;
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

static int
api_bind(lua_State* L)
{
  uint32_t modifiers = 0;

  int num_args = lua_gettop(L);
  for (int i = 1; i < num_args - 1; ++i) {
    const char* lua_modifier = luaL_checkstring(L, i);
    uint32_t modifier = translate_lua_modifier(lua_modifier);

    if (modifier == 0) {
      log_warning("%s", lua_arg_error_msg(L, i, "unknown modifier"));
      return 0;
    }

    modifiers |= modifier;
  }

  // TODO: allow numbers as raw key codes?
  const char* lua_key = luaL_checkstring(L, -2);
  xkb_keysym_t keysym = translate_lua_key(lua_key);

  if (keysym == XKB_KEY_NoSymbol) {
    log_warning("%s", lua_arg_error_msg(L, -2, "unknown key"));
    return 0;
  }

  luaL_checktype(L, -1, LUA_TFUNCTION);
  config_keybindings_bind(modifiers, keysym, luaL_ref(L, LUA_REGISTRYINDEX));

  return 0;
}

static int
api_unbind(lua_State* L)
{
  uint32_t modifiers = 0;

  int num_args = lua_gettop(L);
  for (int i = 1; i < num_args; ++i) {
    const char* lua_modifier = luaL_checkstring(L, i);
    uint32_t modifier = translate_lua_modifier(lua_modifier);

    if (modifier == 0) {
      log_warning("%s", lua_arg_error_msg(L, i, "unknown modifier"));
      return 0;
    }

    modifiers |= modifier;
  }

  // TODO: allow numbers as raw key codes?
  const char* lua_key = luaL_checkstring(L, -1);
  xkb_keysym_t keysym = translate_lua_key(lua_key);

  if (keysym == XKB_KEY_NoSymbol) {
    log_warning("%s", lua_arg_error_msg(L, -1, "unknown key"));
    return 0;
  }

  config_keybindings_unbind(modifiers, keysym);

  return 0;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
api_keybindings_init(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);

  lua_pushcfunction(L, api_bind);
  lua_setfield(L, -2, "bind");

  lua_pushcfunction(L, api_unbind);
  lua_setfield(L, -2, "unbind");

  lua_pop(L, 1);
}
