#include "events.h"
#include "config/events.h"
#include "config/lua_catnip.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

static int
api_publish(lua_State* L)
{
  config_events_publish(luaL_checkstring(L, -1));
  return 0;
}

static int
api_subscribe(lua_State* L)
{
  luaL_checktype(L, -1, LUA_TFUNCTION);
  const char* event = luaL_checkstring(L, -2);
  const int lua_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  config_events_subscribe(event, lua_callback_ref);
  lua_pushnumber(L, lua_callback_ref);
  return 1;
}

static int
api_unsubscribe(lua_State* L)
{
  luaL_checktype(L, -1, LUA_TFUNCTION);
  const char* event = luaL_checkstring(L, -2);
  const int lua_callback_ref = lua_tonumber(L, -1);
  config_events_unsubscribe(event, lua_callback_ref);
  return 0;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
api_events_init(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);

  lua_pushcfunction(L, api_publish);
  lua_setfield(L, -2, "publish");

  lua_pushcfunction(L, api_subscribe);
  lua_setfield(L, -2, "subscribe");

  lua_pushcfunction(L, api_unsubscribe);
  lua_setfield(L, -2, "unsubscribe");

  lua_pop(L, 1);
}
