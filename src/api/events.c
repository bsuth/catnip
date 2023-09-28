#include "events.h"
#include "user_config/events.h"
#include "utils/log.h"
#include "utils/lua.h"
#include <lauxlib.h>

int
api_listen(lua_State* L)
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
  add_event_listener(event, lua_callback_ref);

  lua_pushnumber(L, lua_callback_ref);
  return 1;
}

int
api_unlisten(lua_State* L)
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
  remove_event_listener(event, lua_callback_ref);

  return 0;
}
