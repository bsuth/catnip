#include "lua_events.h"
#include "utils/log.h"
#include <lauxlib.h>

lua_Ref catnip_lua_subscriptions = LUA_NOREF;

void
catnip_lua_events_init(lua_State* L)
{
  lua_newtable(L);
  catnip_lua_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}

void
catnip_lua_events_subscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (!lua_hasfield(L, -1, event)) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, event);
  }

  lua_pushvalue(L, -3);
  lua_pushboolean(L, true);
  lua_rawset(L, -3);

  lua_pop(L, 2);
}

void
catnip_lua_events_unsubscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (lua_hasfield(L, -1, event)) {
    lua_pushvalue(L, -3);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
  }

  lua_pop(L, 1);
}

void
catnip_lua_events_publish(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event,
  int nargs
)
{
  int arg_end = lua_gettop(L) + 1;
  int arg_start = arg_end - nargs;

  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (lua_hasfield(L, -1, event)) {
    // NOTE: We clone the subscriptions table here in order to "freeze" the
    // callbacks. This keeps behavior consistent when a callback itself adds or
    // removes subscriptions (i.e. calls `subscribe` / `unsubscribe`).
    lua_newtable(L);

    lua_pushnil(L);
    while (lua_next(L, -3) != 0) {
      lua_pushvalue(L, -2);
      lua_pushvalue(L, -2);
      lua_rawset(L, -5);
      lua_pop(L, 1);
    }

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
      lua_pushvalue(L, -2);

      for (int i = arg_start; i < arg_end; ++i) {
        lua_pushvalue(L, i);
      }

      if (lua_pcall(L, nargs, 0, 0) != 0) {
        log_error("%s", lua_popstring(L));
      }

      lua_pop(L, 1);
    }

    lua_pop(L, 2);
  }

  lua_pop(L, 1);
}
