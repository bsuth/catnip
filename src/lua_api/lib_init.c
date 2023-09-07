#include "lib_init.h"
#include "../server/display.h"
#include "../user_config/user_config.h"
#include "lua.h"
#include <lauxlib.h>
#include <wayland-server-core.h>

static int
lib_init_quit(lua_State* L)
{
  // TODO: allow user to listen for this event
  wl_display_terminate(server_display);
  return 0;
}

static int
lib_init_reload(lua_State* L)
{
  // TODO: allow user to listen for this event
  user_config_request_reload = true;
  return 0;
}

static const struct luaL_Reg lib_init[] = { { "quit", lib_init_quit },
                                            { "reload", lib_init_reload },
                                            { NULL, NULL } };

void
load_lib_init(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  luaL_newlib(L, lib_init);
  lua_setfield(L, -2, "bwc");
  lua_pop(L, 2);
}
