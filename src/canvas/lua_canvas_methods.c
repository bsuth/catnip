#include "lua_canvas_methods.h"
#include "canvas/canvas.h"
#include "canvas/canvas_methods.h"
#include "lua_resource.h"
#include <lauxlib.h>

int
lua_catnip_canvas_method_clear(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  catnip_canvas_clear(canvas);
  return 0;
}

int
lua_catnip_canvas_method_move(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");

  wlr_scene_node_set_position(
    &canvas->scene_buffer->node,
    luaL_checkinteger(L, 2),
    luaL_checkinteger(L, 3)
  );

  return 0;
}

int
lua_catnip_canvas_method_resize(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  catnip_canvas_resize(canvas, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  return 0;
}

int
lua_catnip_canvas_method_destroy(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  catnip_canvas_destroy(canvas);
  return 0;
}
