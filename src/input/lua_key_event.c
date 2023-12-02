#include "lua_key_event.h"
#include "config/config.h"
#include "events/lua_events.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdlib.h>

static int
lua_catnip_key_event__index(lua_State* L)
{
  struct catnip_key_event** lua_key_event = lua_touserdata(L, 1);
  struct catnip_key_event* key_event = *lua_key_event;

  if (key_event == NULL) {
    lua_log(L, "attempt to index outdated key event");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "code")) {
    lua_pushnumber(L, key_event->keysym);
  } else if (g_str_equal(key, "shift")) {
    lua_pushboolean(catnip_L, key_event->modifiers & WLR_MODIFIER_SHIFT);
  } else if (g_str_equal(key, "ctrl")) {
    lua_pushboolean(catnip_L, key_event->modifiers & WLR_MODIFIER_CTRL);
  } else if (g_str_equal(key, "alt")) {
    lua_pushboolean(catnip_L, key_event->modifiers & WLR_MODIFIER_ALT);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_key_event__newindex(lua_State* L)
{
  struct catnip_key_event** lua_key_event = lua_touserdata(L, 1);
  struct catnip_key_event* key_event = *lua_key_event;

  if (key_event == NULL) {
    lua_log(L, "attempt to index outdated key event");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_log(L, "invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "prevent_notify")) {
    key_event->prevent_notify = lua_toboolean(L, 3);
  } else {
    lua_log(L, "invalid key: %s", key);
  }

  return 0;
}

static int
lua_catnip_key_event__gc(lua_State* L)
{
  struct catnip_key_event** lua_key_event = lua_touserdata(L, 1);
  *lua_key_event = NULL;
  return 0;
}

static const struct luaL_Reg lua_catnip_key_event_mt[] = {
  {"__index", lua_catnip_key_event__index},
  {"__newindex", lua_catnip_key_event__newindex},
  {"__gc", lua_catnip_key_event__gc},
  {NULL, NULL}
};

void
lua_catnip_key_event_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.key.event");
  luaL_setfuncs(L, lua_catnip_key_event_mt, 0);
  lua_pop(L, 1);
}

void
lua_catnip_publish_key_event(struct catnip_key_event* event)
{
  struct catnip_key_event** lua_event =
    lua_newuserdata(catnip_L, sizeof(struct catnip_key_event*));
  luaL_setmetatable(catnip_L, "catnip.key.event");

  *lua_event = event;

  event->wlr_event->state == WL_KEYBOARD_KEY_STATE_PRESSED
    ? lua_catnip_events_call_publish(catnip_L, "keydown", 1)
    : lua_catnip_events_call_publish(catnip_L, "keyup", 1);
}
