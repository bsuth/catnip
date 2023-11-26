#include "lua_window_events.h"
#include "events/lua_events.h"
#include <lauxlib.h>

int
lua_catnip_window_subscribe(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  return catnip_lua_events_local_subscribe(L, (*lua_window)->lua.subscriptions);
}

int
lua_catnip_window_unsubscribe(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  return catnip_lua_events_local_unsubscribe(
    L,
    (*lua_window)->lua.subscriptions
  );
}

int
lua_catnip_window_publish(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  lua_Ref subscriptions = (*lua_window)->lua.subscriptions;
  return catnip_lua_events_local_publish(L, (*lua_window)->lua.subscriptions);
}

void
lua_catnip_window_call_publish(
  lua_State* L,
  struct catnip_window* window,
  const char* event,
  int nargs
)
{
  int call_index = -1 - nargs;
  lua_pushcfunction(L, lua_catnip_window_publish);
  lua_insert(L, call_index);
  lua_rawgeti(L, LUA_REGISTRYINDEX, window->lua.ref);
  lua_insert(L, call_index);
  lua_pushstring(L, event);
  lua_insert(L, call_index);
  lua_call(L, nargs + 2, 0);
}
