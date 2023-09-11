#include "lib_init.h"
#include "lua.h"
#include "server/server.h"
#include "user_config/events.h"
#include "user_config/user_config.h"
#include "utils.h"
#include <glib.h>
#include <lauxlib.h>
#include <wayland-server-core.h>

static int
lib_init_quit(lua_State* L)
{
  stop_server();
  return 0;
}

static int
lib_init_reload(lua_State* L)
{
  user_config_request_reload = true;
  return 0;
}

static int
lib_init_add_event_listener(lua_State* L)
{
  if (lua_type(L, -2) != LUA_TSTRING) {
    g_warning("%s", get_arg_type_error_msg(L, -2, LUA_TSTRING));
    return 0;
  }

  if (lua_type(L, -1) != LUA_TFUNCTION) {
    g_warning("%s", get_arg_type_error_msg(L, -1, LUA_TFUNCTION));
    return 0;
  }

  const char* event = lua_tostring(L, -2);
  const int lua_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  add_event_listener(event, lua_callback_ref);

  lua_pushnumber(L, lua_callback_ref);
  return 1;
}

static int
lib_init_remove_event_listener(lua_State* L)
{
  if (lua_type(L, -2) != LUA_TSTRING) {
    g_warning("%s", get_arg_type_error_msg(L, -2, LUA_TSTRING));
    return 0;
  }

  if (lua_type(L, -1) != LUA_TNUMBER) {
    g_warning("%s", get_arg_type_error_msg(L, -1, LUA_TNUMBER));
    return 0;
  }

  const char* event = lua_tostring(L, -2);
  const int lua_callback_ref = lua_tonumber(L, -1);
  remove_event_listener(event, lua_callback_ref);

  return 0;
}

static const struct luaL_Reg lib_init[] = {
  {"quit", lib_init_quit},
  {"reload", lib_init_reload},
  {"add_event_listener", lib_init_add_event_listener},
  {"remove_event_listener", lib_init_remove_event_listener},
  {NULL, NULL}};

void
load_lib_init(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  luaL_newlib(L, lib_init);
  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);
}
