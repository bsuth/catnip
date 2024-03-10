#include "lua_keyboard_key_event.h"
#include "keyboard/keyboard.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>

static int
lua_catnip_keyboard_key_event__index(lua_State* L)
{
  struct catnip_keyboard_key_event** lua_key_event = lua_touserdata(L, 1);
  struct catnip_keyboard_key_event* key_event = *lua_key_event;

  if (key_event == NULL) {
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "code")) {
    lua_pushnumber(L, key_event->xkb_keysym);
  } else if (streq(key, "name")) {
    lua_pushstring(L, key_event->xkb_name);
  } else if (streq(key, "shift")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_SHIFT);
  } else if (streq(key, "ctrl")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_CTRL);
  } else if (streq(key, "mod1")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_ALT);
  } else if (streq(key, "mod2")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD2);
  } else if (streq(key, "mod3")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD3);
  } else if (streq(key, "mod4")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_LOGO);
  } else if (streq(key, "mod5")) {
    lua_pushboolean(L, key_event->modifiers & WLR_MODIFIER_MOD5);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_keyboard_key_event__newindex(lua_State* L)
{
  struct catnip_keyboard_key_event** lua_key_event = lua_touserdata(L, 1);
  struct catnip_keyboard_key_event* key_event = *lua_key_event;

  if (key_event == NULL) {
    lua_log_error(L, "attempt to index outdated userdata");
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "prevent_notify")) {
    key_event->prevent_notify = lua_toboolean(L, 3);
  } else {
    lua_log_error(L, "unknown userdata field (%s)", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_keyboard_key_event_mt[] = {
  {"__index", lua_catnip_keyboard_key_event__index},
  {"__newindex", lua_catnip_keyboard_key_event__newindex},
  {NULL, NULL}
};

void
lua_catnip_keyboard_key_event_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.keyboard.key.event");
  luaL_setfuncs(L, lua_catnip_keyboard_key_event_mt, 0);
  lua_pop(L, 1);
}
