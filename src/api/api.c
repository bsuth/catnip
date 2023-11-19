#include "api.h"
#include "api/canvas.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/png.h"
#include "api/svg.h"
#include "config/config.h"
#include "cursor/lua_cursor.h"
#include "output/lua_output.h"
#include "server/server.h"
#include "window/lua_window.h"
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

  lua_catnip_window_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 1);

  lua_catnip_output_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_outputs);
  lua_setfield(L, -2, "outputs");
  lua_pop(L, 1);

  lua_catnip_cursor_init(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_cursor);
  lua_setfield(L, -2, "cursor");
  lua_pop(L, 1);

  api_keybindings_init(L);
  api_events_init(L);
  api_canvas_init(L);
  api_svg_init(L);
  api_png_init(L);
}
