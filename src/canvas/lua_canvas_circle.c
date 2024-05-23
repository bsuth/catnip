#include "lua_canvas_circle.h"
#include "canvas/canvas.h"
#include "utils/cairo.h"
#include <lauxlib.h>

int
lua_catnip_canvas_circle(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");
  luaL_checktype(L, 2, LUA_TTABLE);

  cairo_save(canvas->cr);

  int x = lua_hasnumberfield(L, 2, "x") ? lua_popnumber(L) : 0;
  int y = lua_hasnumberfield(L, 2, "y") ? lua_popnumber(L) : 0;
  int radius = lua_hasnumberfield(L, 2, "radius") ? lua_popnumber(L) : 0;

  cairo_arc(canvas->cr, x, y, radius, 0, 2 * M_PI);

  int fill_color =
    lua_hasnumberfield(L, 2, "fill_color") ? lua_popinteger(L) : -1;
  double fill_opacity =
    lua_hasnumberfield(L, 2, "fill_opacity") ? lua_popnumber(L) : 1;

  if (fill_color != -1 && fill_opacity > 0) {
    cairo_set_source_hexa(canvas->cr, fill_color, fill_opacity);
    cairo_fill_preserve(canvas->cr);
  }

  int stroke_color =
    lua_hasnumberfield(L, 2, "stroke_color") ? lua_popinteger(L) : -1;
  double stroke_opacity =
    lua_hasnumberfield(L, 2, "stroke_opacity") ? lua_popnumber(L) : 1;
  double stroke_size =
    lua_hasnumberfield(L, 2, "stroke_size") ? lua_popnumber(L) : 1;

  if (stroke_color != -1 && stroke_opacity > 0 && stroke_size > 0) {
    cairo_set_line_width(canvas->cr, stroke_size);
    cairo_set_source_hexa(canvas->cr, stroke_color, stroke_opacity);
    cairo_stroke_preserve(canvas->cr);
  }

  cairo_new_path(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);

  return 0;
}
