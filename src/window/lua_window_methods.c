#include "lua_window_methods.h"
#include "lua_resource.h"
#include "window/window.h"
#include <lauxlib.h>
#include <wlr/types/wlr_scene.h>

int
lua_catnip_window_method_move(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkname(L, 1, "window");

  wlr_scene_node_set_position(
    &window->scene_tree->node,
    luaL_checkinteger(L, 2),
    luaL_checkinteger(L, 3)
  );

  return 0;
}

int
lua_catnip_window_method_resize(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkname(L, 1, "window");

  wlr_xdg_toplevel_set_size(
    window->xdg_toplevel,
    luaL_checkinteger(L, 2),
    luaL_checkinteger(L, 3)
  );

  return 0;
}

int
lua_catnip_window_method_destroy(lua_State* L)
{
  struct catnip_window* window = lua_catnip_resource_checkname(L, 1, "window");
  wlr_xdg_toplevel_send_close(window->xdg_toplevel);
  return 0;
}
