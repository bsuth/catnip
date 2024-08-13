#include "lua_window_list.h"
#include "desktop/lua_window.h"
#include "desktop/windows.h"

struct catnip_lua_resource_list* lua_catnip_window_list = NULL;

void
lua_catnip_window_list_init(lua_State* L)
{
  lua_catnip_window_list = lua_catnip_resource_list_create(L);
}

void
lua_catnip_window_list_populate(lua_State* L)
{
  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    // Do not create the Lua window unless the surface has actually been mapped,
    // i.e. initialization has finished and the window is ready to be shown on
    // screen.
    if (window->wlr.xdg_toplevel->base->surface->mapped) {
      lua_catnip_window_create(L, window);
    }
  }
}
