#include "api.h"
#include "api/cursor.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/monitor.h"
#include "api/refs.h"
#include "api/ui/ui.h"
#include "api/window.h"
#include "server/server.h"
#include "user_config/user_config.h"
#include <lauxlib.h>

static int
api_quit(lua_State* L)
{
  stop_server();
  return 0;
}

static int
api_reload(lua_State* L)
{
  user_config_request_reload = true;
  return 0;
}

static const struct luaL_Reg lib_init[] = {
  {"quit", api_quit},
  {"reload", api_reload},
  {"publish", api_publish},
  {"subscribe", api_subscribe},
  {"unsubscribe", api_unsubscribe},
  {"bind", api_bind},
  {"unbind", api_unbind},
  {NULL, NULL}};

void
init_api(lua_State* L)
{
  luaL_newlib(L, lib_init);
  api_catnip = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);

  init_api_windows(L);
  init_api_monitors(L);
  init_api_cursor(L);
  init_api_ui(L);
}
