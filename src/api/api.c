#include "api.h"
#include "api/cursor.h"
#include "api/events.h"
#include "api/keybindings.h"
#include "api/monitor.h"
#include "api/window.h"
#include "server/server.h"
#include "ui/drawable.h"
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

static int
lib_test(lua_State* L)
{
  struct drawable* drawable = create_drawable();
  cairo_set_source_rgba(drawable->cr, 1, 0, 0, 1);
  cairo_rectangle(drawable->cr, 0, 0, 500, 500);
  cairo_fill(drawable->cr);
  return 0;
}

static const struct luaL_Reg lib_init[] = {
  {"quit", api_quit},
  {"reload", api_reload},
  {"listen", api_listen},
  {"unlisten", api_unlisten},
  {"bind", api_bind},
  {"unbind", api_unbind},
  {"test", lib_test},
  {NULL, NULL}};

void
init_api(lua_State* L)
{
  luaL_newlib(L, lib_init);
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_pushvalue(L, -3);
  lua_setfield(L, -2, "catnip");
  lua_pop(L, 2);

  init_api_windows(L);
  init_api_monitors(L);
  init_api_cursor(L);
}
