#include "lua_events.h"
#include "utils/log.h"
#include <lauxlib.h>

lua_Ref lua_catnip_subscriptions = LUA_NOREF;

void
lua_catnip_events_subscribe(
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

  size_t num_subscriptions = lua_objlen(L, -1);

  for (int i = 0; i < num_subscriptions; ++i) {
    lua_rawgeti(L, -1, i + 1);

    if (lua_equal(L, -4, -1)) {
      lua_pop(L, 3);
      return; // subscription already exists
    }

    lua_pop(L, 1);
  }

  lua_pushvalue(L, -4);
  lua_rawseti(L, -2, num_subscriptions + 1);
  lua_pop(L, 2);
}

void
lua_catnip_events_unsubscribe(
  lua_State* L,
  lua_Ref subscriptions,
  const char* event
)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (lua_hasfield(L, -1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);
    bool found_lua_subscription = false;

    for (int i = 0; i < num_subscriptions; ++i) {
      if (found_lua_subscription) {
        lua_rawgeti(L, -1, i + 1);
        lua_rawseti(L, -2, i);
      } else {
        lua_rawgeti(L, -1, i + 1);
        found_lua_subscription = lua_equal(L, -4, -1);
        lua_pop(L, 1);
      }
    }

    if (found_lua_subscription) {
      lua_pushnil(L);
      lua_rawseti(L, -2, num_subscriptions);
    }

    lua_pop(L, 1);
  }

  lua_pop(L, 1);
}

void
lua_catnip_events_publish(
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
    size_t num_subscriptions = lua_objlen(L, -1);

    // NOTE: We clone the subscriptions table here in order to "freeze" the
    // callbacks. This keeps behavior consistent when a callback itself adds or
    // removes subscriptions (i.e. calls `subscribe` / `unsubscribe`).
    lua_newtable(L);

    for (int i = 0; i < num_subscriptions; ++i) {
      lua_rawgeti(L, -2, i + 1);
      lua_rawseti(L, -2, i + 1);
    }

    for (int i = 0; i < num_subscriptions; ++i) {
      lua_rawgeti(L, -1, i + 1);

      for (int j = arg_start; j < arg_end; ++j) {
        lua_pushvalue(L, j);
      }

      if (lua_pcall(L, nargs, 0, 0) != 0) {
        log_error("%s", lua_popstring(L));
      }
    }

    lua_pop(L, 2);
  }

  lua_pop(L, 1);
}

void
lua_catnip_events_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}
