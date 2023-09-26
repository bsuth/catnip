#include "lib_keybindings.h"
#include "lua_api/utils.h"
#include "user_config/keybindings.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>
#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>

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

int
lib_keybindings_add(lua_State* L)
{
  uint32_t modifiers = 0;

  int num_args = lua_gettop(L);
  for (int i = 1; i < num_args - 1; ++i) {
    if (lua_type(L, i) != LUA_TSTRING) {
      log_warning("%s", get_arg_type_error_msg(L, i, LUA_TSTRING));
      return 0;
    }

    const char* lua_modifier = lua_tostring(L, i);
    uint32_t modifier = translate_lua_modifier(lua_modifier);

    if (modifier == 0) {
      log_warning("%s", get_arg_error_msg(L, i, "unknown modifier"));
      return 0;
    }

    modifiers |= modifier;
  }

  // TODO: allow numbers as raw key codes?
  if (lua_type(L, -2) != LUA_TSTRING) {
    log_warning("%s", get_arg_type_error_msg(L, -2, LUA_TSTRING));
    return 0;
  }

  const char* lua_key = lua_tostring(L, -2);
  xkb_keysym_t keysym = translate_lua_key(lua_key);

  if (keysym == XKB_KEY_NoSymbol) {
    log_warning("%s", get_arg_error_msg(L, -2, "unknown key"));
    return 0;
  }

  if (lua_type(L, -1) != LUA_TFUNCTION) {
    log_warning("%s", get_arg_type_error_msg(L, -1, LUA_TFUNCTION));
    return 0;
  }

  add_user_keybinding(modifiers, keysym, luaL_ref(L, LUA_REGISTRYINDEX));

  return 0;
}

int
lib_keybindings_remove(lua_State* L)
{
  uint32_t modifiers = 0;

  int num_args = lua_gettop(L);
  for (int i = 1; i < num_args; ++i) {
    if (lua_type(L, i) != LUA_TSTRING) {
      log_warning("%s", get_arg_type_error_msg(L, i, LUA_TSTRING));
      return 0;
    }

    const char* lua_modifier = lua_tostring(L, i);
    uint32_t modifier = translate_lua_modifier(lua_modifier);

    if (modifier == 0) {
      log_warning("%s", get_arg_error_msg(L, i, "unknown modifier"));
      return 0;
    }

    modifiers |= modifier;
  }

  // TODO: allow numbers as raw key codes?
  if (lua_type(L, -1) != LUA_TSTRING) {
    log_warning("%s", get_arg_type_error_msg(L, -1, LUA_TSTRING));
    return 0;
  }

  const char* lua_key = lua_tostring(L, -1);
  xkb_keysym_t keysym = translate_lua_key(lua_key);

  if (keysym == XKB_KEY_NoSymbol) {
    log_warning("%s", get_arg_error_msg(L, -1, "unknown key"));
    return 0;
  }

  remove_user_keybinding(modifiers, keysym);

  return 0;
}

int
lib_keybindings_clear(lua_State* L)
{
  clear_user_keybindings();
  return 0;
}
