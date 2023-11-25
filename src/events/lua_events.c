#include "lua_events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

lua_Ref lua_catnip_subscriptions = LUA_NOREF;

int
catnip_lua_events_subscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);

  if (!lua_hasfield(L, -1, event)) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, event);
  }

  size_t num_subscriptions = lua_objlen(L, -1);

  for (int i = 1; i <= num_subscriptions; ++i) {
    lua_rawgeti(L, -1, i);

    if (lua_equal(L, 2, -1)) {
      lua_pop(L, 3);
      return 1; // subscription already exists
    }

    lua_pop(L, 1);
  }

  lua_pushvalue(L, 2);
  lua_rawseti(L, -2, num_subscriptions + 1);
  lua_pop(L, 2);

  lua_pushvalue(L, 2);
  return 1;
}

int
catnip_lua_events_unsubscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);

  if (lua_hasfield(L, -1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);
    bool found_lua_subscription = false;

    for (int i = 1; i <= num_subscriptions; ++i) {
      if (found_lua_subscription) {
        lua_rawgeti(L, -1, i);
        lua_rawseti(L, -2, i - 1);
      } else {
        lua_rawgeti(L, -1, i);
        found_lua_subscription = lua_equal(L, 2, -1);
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
  return 0;
}

int
catnip_lua_events_publish(lua_State* L)
{
  const int num_args = lua_gettop(L);
  const char* event = luaL_checkstring(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);

  if (lua_hasfield(L, -1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);

    for (int sub_index = 1; sub_index <= num_subscriptions; ++sub_index) {
      lua_rawgeti(L, -1, sub_index);

      for (int arg_index = 2; arg_index <= num_args; ++arg_index) {
        lua_pushvalue(L, arg_index);
      }

      if (lua_pcall(L, num_args - 1, 0, 0) != 0) {
        log_error("%s", lua_tostring(L, -1));
      }
    }

    lua_pop(L, 1);
  }

  lua_pop(L, 1);
  return 0;
}

void
catnip_lua_events_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}
