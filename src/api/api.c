#include "api.h"
#include "api/cursor.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/output.h"
#include "api/ui/ui.h"
#include "api/window.h"
#include "config/config.h"
#include "server/server.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

lua_Ref api_catnip = LUA_NOREF;

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

static int
api_quit(lua_State* L)
{
  stop_server();
  return 0;
}

static int
api_reload(lua_State* L)
{
  config_reload_requested = true;
  return 0;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

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
  init_api_outputs(L);
  init_api_cursor(L);
  init_api_ui(L);
}
