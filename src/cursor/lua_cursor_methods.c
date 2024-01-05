#include "lua_cursor_methods.h"
#include "cursor/lua_cursor.h"
#include "events/lua_events.h"
#include <lauxlib.h>

int
lua_catnip_cursor_method_subscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, lua_catnip_cursor_subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

int
lua_catnip_cursor_method_unsubscribe(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, lua_catnip_cursor_subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

int
lua_catnip_cursor_method_publish(lua_State* L)
{
  luaL_checkudata(L, 1, "catnip.cursor");
  const char* event = luaL_checkstring(L, 2);
  lua_catnip_events_publish(L, lua_catnip_cursor_subscriptions, event, 1);
  return 0;
}
