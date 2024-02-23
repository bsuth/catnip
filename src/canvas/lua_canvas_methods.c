#include "lua_canvas_methods.h"
#include "canvas/canvas_methods.h"
#include <cairo.h>
#include <lauxlib.h>

int
lua_catnip_canvas_clear(lua_State* L)
{
  struct catnip_canvas** lua_canvas = luaL_checkudata(L, 1, "catnip.canvas");
  struct catnip_canvas* canvas = *lua_canvas;

  cairo_save(canvas->cr);
  cairo_set_operator(canvas->cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);

  return 0;
}
