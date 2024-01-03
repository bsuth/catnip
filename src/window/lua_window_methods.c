#include "lua_window_methods.h"
#include "window.h"
#include <lauxlib.h>
#include <wlr/types/wlr_xdg_shell.h>

// TODO: merge this and `lua_window_events`?

int
lua_catnip_window_method_destroy(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  wlr_xdg_toplevel_send_close((*lua_window)->xdg_toplevel);
  return 0;
}
