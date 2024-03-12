#include "lua_window_list.h"
#include "lua_resource_list.h"
#include "window/lua_window.h"
#include <lauxlib.h>

struct catnip_lua_resource_list* lua_catnip_window_list = NULL;

void
lua_catnip_window_list_init(lua_State* L)
{
  lua_catnip_window_list = lua_catnip_resource_list_create(L);
  lua_catnip_window_list->__destroy = lua_catnip_window_destroy;

  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    lua_catnip_window_create(L, window);
  }
}
