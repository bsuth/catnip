#include "lua_canvas_methods.h"
#include "canvas/canvas.h"
#include "canvas/canvas_methods.h"
#include "canvas/lua_png.h"
#include "canvas/lua_svg.h"
#include "lua_resource.h"
#include <lauxlib.h>

int
lua_catnip_canvas_method_png(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  struct lua_catnip_png* lua_png = luaL_checkudata(L, 2, "catnip.png");
  luaL_checktype(L, 3, LUA_TTABLE);

  lua_catnip_png_render(
    lua_png,
    canvas,
    lua_hasfield(L, 3, "x") ? lua_popnumber(L) : 0,
    lua_hasfield(L, 3, "y") ? lua_popnumber(L) : 0,
    lua_hasfield(L, 3, "width") ? lua_popnumber(L) : -1,
    lua_hasfield(L, 3, "height") ? lua_popnumber(L) : -1
  );

  return 0;
}

int
lua_catnip_canvas_method_svg(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  struct lua_catnip_svg* lua_svg = luaL_checkudata(L, 2, "catnip.svg");
  luaL_checktype(L, 3, LUA_TTABLE);

  lua_catnip_svg_render(
    lua_svg,
    canvas,
    lua_hasfield(L, 3, "x") ? lua_popnumber(L) : 0,
    lua_hasfield(L, 3, "y") ? lua_popnumber(L) : 0,
    lua_hasfield(L, 3, "width") ? lua_popnumber(L) : -1,
    lua_hasfield(L, 3, "height") ? lua_popnumber(L) : -1
  );

  return 0;
}

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
