#include "lua_output_events.h"
#include "events/lua_events.h"
#include <lauxlib.h>

int
lua_catnip_output_subscribe(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  return catnip_lua_events_local_subscribe(L, (*lua_output)->lua.subscriptions);
}

int
lua_catnip_output_unsubscribe(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  return catnip_lua_events_local_unsubscribe(
    L,
    (*lua_output)->lua.subscriptions
  );
}

int
lua_catnip_output_publish(lua_State* L)
{
  struct catnip_output** lua_output = luaL_checkudata(L, 1, "catnip.output");
  lua_Ref subscriptions = (*lua_output)->lua.subscriptions;
  return catnip_lua_events_local_publish(L, (*lua_output)->lua.subscriptions);
}

void
lua_catnip_output_call_publish(
  lua_State* L,
  struct catnip_output* output,
  const char* event,
  int nargs
)
{
  int call_index = -1 - nargs;
  lua_pushcfunction(L, lua_catnip_output_publish);
  lua_insert(L, call_index);
  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
  lua_insert(L, call_index);
  lua_pushstring(L, event);
  lua_insert(L, call_index);
  lua_call(L, nargs + 2, 0);
}
