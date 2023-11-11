#include "api.h"
#include "api/canvas.h"
#include "api/cursor.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/output.h"
#include "api/png.h"
#include "api/svg.h"
#include "api/window.h"
#include "config/config.h"
#include "server/server.h"
#include <lauxlib.h>

lua_Ref api_ref = LUA_NOREF;

// -----------------------------------------------------------------------------
// Lib
// -----------------------------------------------------------------------------

static int
api_quit(lua_State* L)
{
  server_stop();
  return 0;
}

static int
api_reload(lua_State* L)
{
  config_reload_requested = true;
  return 0;
}

static const struct luaL_Reg api_lib[] =
  {{"quit", api_quit}, {"reload", api_reload}, {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
api_init(lua_State* L)
{
  luaL_newlib(L, api_lib);
  api_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);

  api_keybindings_init(L);
  api_events_init(L);
  api_window_init(L);
  api_output_init(L);
  api_cursor_init(L);
  api_canvas_init(L);
  api_svg_init(L);
  api_png_init(L);
}
