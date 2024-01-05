#include "lua_keyboard_methods.h"
#include "events/lua_events.h"
#include "keyboard/keyboard.h"
#include "keyboard/lua_keyboard.h"
#include <lauxlib.h>

int
lua_catnip_keyboard_method_subscribe(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, (*lua_keyboard)->lua.subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

int
lua_catnip_keyboard_method_unsubscribe(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, (*lua_keyboard)->lua.subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

int
lua_catnip_keyboard_method_publish(lua_State* L)
{
  struct catnip_keyboard** lua_keyboard =
    luaL_checkudata(L, 1, "catnip.keyboard");
  const char* event = luaL_checkstring(L, 2);
  lua_catnip_keyboard_publish(L, *lua_keyboard, event, lua_gettop(L) - 2);
  return 0;
}
