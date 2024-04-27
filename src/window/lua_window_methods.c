#include "lua_window_methods.h"
#include "lua_resource.h"
#include "window/window.h"

int
lua_catnip_window_method_destroy(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkname(L, 1, "window");
  wlr_xdg_toplevel_send_close(window->xdg_toplevel);
  return 0;
}
