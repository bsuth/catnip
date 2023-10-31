#include "events.h"
#include "api/api.h"
#include "config/events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

static int
api_publish(lua_State* L)
{
  if (lua_type(L, -1) != LUA_TSTRING) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -1, LUA_TSTRING));
    return 0;
  }

  config_events_publish(lua_tostring(L, -1));
  return 0;
}

static int
api_subscribe(lua_State* L)
{
  if (lua_type(L, -2) != LUA_TSTRING) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -2, LUA_TSTRING));
    return 0;
  }

  if (lua_type(L, -1) != LUA_TFUNCTION) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -1, LUA_TFUNCTION));
    return 0;
  }

  const char* event = lua_tostring(L, -2);
  const int lua_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  config_events_subscribe(event, lua_callback_ref);

  lua_pushnumber(L, lua_callback_ref);
  return 1;
}

static int
api_unsubscribe(lua_State* L)
{
  if (lua_type(L, -2) != LUA_TSTRING) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -2, LUA_TSTRING));
    return 0;
  }

  if (lua_type(L, -1) != LUA_TNUMBER) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -1, LUA_TNUMBER));
    return 0;
  }

  const char* event = lua_tostring(L, -2);
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
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);

  lua_pushcfunction(L, api_publish);
  lua_setfield(L, -2, "publish");

  lua_pushcfunction(L, api_subscribe);
  lua_setfield(L, -2, "subscribe");

  lua_pushcfunction(L, api_unsubscribe);
  lua_setfield(L, -2, "unsubscribe");

  lua_pop(L, 1);
}
