#include "lua_events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

lua_Ref lua_catnip_subscriptions = LUA_NOREF;

int
catnip_lua_events_subscribe(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  if (!lua_hasfield(L, 1, event)) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, 1, event);
  }

  size_t num_subscriptions = lua_objlen(L, -1);

  for (int i = 1; i <= num_subscriptions; ++i) {
    lua_rawgeti(L, -1, i);

    if (lua_equal(L, 3, -1)) {
      lua_remove(L, -2);
      return 1; // subscription already exists
    }

    lua_pop(L, 1);
  }

  lua_pushvalue(L, 3);
  lua_rawseti(L, -2, num_subscriptions + 1);
  lua_pop(L, 1);

  lua_pushvalue(L, 3);
  return 1;
}

int
catnip_lua_events_global_subscribe(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);
  lua_insert(L, 1);
  return catnip_lua_events_subscribe(L);
}

int
catnip_lua_events_unsubscribe(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  if (lua_hasfield(L, 1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);
    bool found_lua_subscription = false;

    for (int i = 1; i <= num_subscriptions; ++i) {
      if (found_lua_subscription) {
        lua_rawgeti(L, -1, i);
        lua_rawseti(L, -2, i - 1);
      } else {
        lua_rawgeti(L, -1, i);
        found_lua_subscription = lua_equal(L, 3, -1);
        lua_pop(L, 1);
      }
    }

    if (found_lua_subscription) {
      lua_pushnil(L);
      lua_rawseti(L, -2, num_subscriptions);
    }

    lua_pop(L, 1);
  }

  return 0;
}

int
catnip_lua_events_global_unsubscribe(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);
  lua_insert(L, 1);
  return catnip_lua_events_unsubscribe(L);
}

int
catnip_lua_events_publish(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  const char* event = luaL_checkstring(L, 2);
  const int top = lua_gettop(L);

  if (lua_hasfield(L, 1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);

    for (int i = 1; i <= num_subscriptions; ++i) {
      lua_rawgeti(L, -1, i);

      for (int j = 3; j <= top; ++j) {
        lua_pushvalue(L, j);
      }

      if (lua_pcall(L, top - 2, 0, 0) != 0) {
        log_error("%s", lua_popstring(L));
      }
    }

    lua_pop(L, 1);
  }

  return 0;
}

int
catnip_lua_events_global_publish(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);
  lua_insert(L, 1);
  return catnip_lua_events_publish(L);
}

void
lua_catnip_events_call_publish(lua_State* L, const char* event, int nargs)
{
  lua_pushcfunction(L, catnip_lua_events_global_publish);
  lua_insert(L, -1 - nargs);
  lua_pushstring(L, event);
  lua_insert(L, -1 - nargs);
  lua_call(L, nargs + 1, 0);
}

void
catnip_lua_events_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}
