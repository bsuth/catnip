#include "lua_events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

lua_Ref lua_catnip_subscriptions = LUA_NOREF;

static int
catnip_lua_events_subscribe(
  lua_State* L,
  int event_index,
  int callback_index,
  lua_Ref subscriptions
)
{
  const char* event = luaL_checkstring(L, event_index);
  luaL_checktype(L, callback_index, LUA_TFUNCTION);

  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (!lua_hasfield(L, -1, event)) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, event);
  }

  size_t num_subscriptions = lua_objlen(L, -1);

  for (int i = 1; i <= num_subscriptions; ++i) {
    lua_rawgeti(L, -1, i);

    if (lua_equal(L, callback_index, -1)) {
      lua_pop(L, 3);
      return 1; // subscription already exists
    }

    lua_pop(L, 1);
  }

  lua_pushvalue(L, callback_index);
  lua_rawseti(L, -2, num_subscriptions + 1);
  lua_pop(L, 2);

  lua_pushvalue(L, callback_index);
  return 1;
}

int
catnip_lua_events_local_subscribe(lua_State* L, lua_Ref subscriptions)
{
  return catnip_lua_events_subscribe(L, 2, 3, subscriptions);
}

int
catnip_lua_events_global_subscribe(lua_State* L)
{
  return catnip_lua_events_subscribe(L, 1, 2, lua_catnip_subscriptions);
}

static int
catnip_lua_events_unsubscribe(
  lua_State* L,
  int event_index,
  int callback_index,
  lua_Ref subscriptions
)
{
  const char* event = luaL_checkstring(L, event_index);
  luaL_checktype(L, callback_index, LUA_TFUNCTION);

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
        found_lua_subscription = lua_equal(L, callback_index, -1);
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
catnip_lua_events_local_unsubscribe(lua_State* L, lua_Ref subscriptions)
{
  return catnip_lua_events_unsubscribe(L, 2, 3, subscriptions);
}

int
catnip_lua_events_global_unsubscribe(lua_State* L)
{
  return catnip_lua_events_unsubscribe(L, 1, 2, lua_catnip_subscriptions);
}

static int
catnip_lua_events_publish(lua_State* L, int event_index, lua_Ref subscriptions)
{
  const int num_args = lua_gettop(L);
  const char* event = luaL_checkstring(L, event_index);

  lua_rawgeti(L, LUA_REGISTRYINDEX, subscriptions);

  if (lua_hasfield(L, -1, event)) {
    size_t num_subscriptions = lua_objlen(L, -1);

    for (int subscription_index = 1; subscription_index <= num_subscriptions;
         ++subscription_index) {
      lua_rawgeti(L, -1, subscription_index);

      for (int arg_index = 1; arg_index <= num_args; ++arg_index) {
        if (arg_index != event_index) {
          lua_pushvalue(L, arg_index);
        }
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

int
catnip_lua_events_local_publish(lua_State* L, lua_Ref subscriptions)
{
  return catnip_lua_events_publish(L, 2, subscriptions);
}

int
catnip_lua_events_global_publish(lua_State* L)
{
  return catnip_lua_events_publish(L, 1, lua_catnip_subscriptions);
}

void
lua_catnip_events_call_publish(lua_State* L, const char* event, int nargs)
{
  int call_index = -1 - nargs;
  lua_pushcfunction(L, catnip_lua_events_global_publish);
  lua_insert(L, call_index);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_subscriptions);
  lua_insert(L, call_index);
  lua_pushstring(L, event);
  lua_insert(L, call_index);
  lua_call(L, nargs + 1, 0);
}

void
catnip_lua_events_init(lua_State* L)
{
  lua_newtable(L);
  lua_catnip_subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);
}
