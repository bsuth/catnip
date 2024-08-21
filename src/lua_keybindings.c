#include "lua_keybindings.h"
#include "extensions/string.h"
#include <lauxlib.h>
#include <stdint.h>
#include <string.h>
#include <wlr/types/wlr_keyboard.h>
#include <xkbcommon/xkbcommon.h>

lua_Ref catnip_lua_keybindings;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static uint32_t
catnip_lua_keybindings_get_modifiers(lua_State* L, int idx)
{
  uint32_t modifiers = 0;
  size_t objlen = lua_objlen(L, idx);

  for (size_t i = 0; i < objlen; ++i) {
    lua_rawgeti(L, idx, i + 1);
    const char* modifier = lua_tostring(L, -1);

    if (modifier == NULL) {
      continue;
    }

    if (streq(modifier, "ctrl")) {
      modifiers |= WLR_MODIFIER_CTRL;
    } else if (streq(modifier, "mod1")) {
      modifiers |= WLR_MODIFIER_ALT;
    } else if (streq(modifier, "mod2")) {
      modifiers |= WLR_MODIFIER_MOD2;
    } else if (streq(modifier, "mod3")) {
      modifiers |= WLR_MODIFIER_MOD3;
    } else if (streq(modifier, "mod4")) {
      modifiers |= WLR_MODIFIER_LOGO;
    } else if (streq(modifier, "mod5")) {
      modifiers |= WLR_MODIFIER_MOD5;
    }

    lua_pop(L, 1);
  }

  return modifiers;
}

static xkb_keysym_t
catnip_lua_keybindings_get_xkb_keysym(lua_State* L, const char* key)
{
  if (key == NULL) {
    return XKB_KEY_NoSymbol;
  } else if (strlen(key) == 1) {
    xkb_keysym_t xkb_keysym = (xkb_keysym_t) key[0];
    return 31 < xkb_keysym && xkb_keysym < 127 ? xkb_keysym : XKB_KEY_NoSymbol;
  } else {
    return xkb_keysym_from_name(key, 0);
  }
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_keybindings_init(lua_State* L)
{
  lua_newtable(L);
  catnip_lua_keybindings = luaL_ref(L, LUA_REGISTRYINDEX);
}

bool
catnip_lua_keybindings_check(lua_State* L, struct catnip_key_event* key_event)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_keybindings);
  lua_rawgeti(L, -1, key_event->xkb_keysym);

  if (lua_isnil(L, -1)) {
    lua_pop(L, 2);
    return false;
  }

  uint32_t bindable_modifiers = WLR_MODIFIER_CTRL | WLR_MODIFIER_ALT
    | WLR_MODIFIER_MOD2 | WLR_MODIFIER_MOD3 | WLR_MODIFIER_LOGO
    | WLR_MODIFIER_MOD5;

  lua_rawgeti(L, -1, key_event->modifiers & bindable_modifiers);

  if (lua_isnil(L, -1)) {
    lua_pop(L, 3);
    return false;
  }

  if (key_event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    if (lua_pcall(L, 0, 0, 0) != 0) {
      log_error("%s", lua_popstring(L));
    }
  }

  lua_pop(L, 2);
  return true;
}

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

int
catnip_lua_keybindings_bind(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  const char* key = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uint32_t modifiers = catnip_lua_keybindings_get_modifiers(L, 1);
  xkb_keysym_t xkb_keysym = catnip_lua_keybindings_get_xkb_keysym(L, key);

  if (xkb_keysym == XKB_KEY_NoSymbol) {
    lua_log_error(L, "unknown key: %s", key);
    return 0;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_keybindings);
  lua_rawgeti(L, -1, xkb_keysym);

  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_rawseti(L, -3, xkb_keysym);
  }

  lua_pushvalue(L, 3);
  lua_rawseti(L, -2, modifiers);

  lua_pop(L, 2);
  return 0;
}

int
catnip_lua_keybindings_unbind(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  const char* key = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uint32_t modifiers = catnip_lua_keybindings_get_modifiers(L, 1);
  uint32_t xkb_keysym = catnip_lua_keybindings_get_xkb_keysym(L, key);

  if (xkb_keysym == XKB_KEY_NoSymbol) {
    return 0;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_keybindings);
  lua_rawgeti(L, -1, xkb_keysym);

  if (lua_isnil(L, -1)) {
    lua_pop(L, 2);
    return 0;
  }

  lua_pushnil(L);
  lua_rawseti(L, -2, modifiers);

  lua_pop(L, 2);
  return 0;
}
