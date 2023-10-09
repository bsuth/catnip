#include "events.h"
#include "config/events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

int
api_publish(lua_State* L)
{
  if (lua_type(L, -1) != LUA_TSTRING) {
    log_warning("%s", lua_get_arg_type_error_msg(L, -1, LUA_TSTRING));
    return 0;
  }

  publish_config_event(lua_tostring(L, -1));
  return 0;
}

int
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
  subscribe_config_event(event, lua_callback_ref);

  lua_pushnumber(L, lua_callback_ref);
  return 1;
}

int
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
  unsubscribe_config_event(event, lua_callback_ref);

  return 0;
}
