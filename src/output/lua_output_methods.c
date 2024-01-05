#include "lua_output_methods.h"
#include "events/lua_events.h"
#include "output/lua_output.h"
#include <lauxlib.h>

int
lua_catnip_output_method_subscribe(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, (*lua_output)->lua.subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

int
lua_catnip_output_method_unsubscribe(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, (*lua_output)->lua.subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

int
lua_catnip_output_method_publish(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  const char* event = luaL_checkstring(L, 2);
  lua_catnip_output_publish(L, *lua_output, event, lua_gettop(L) - 2);
  return 0;
}
