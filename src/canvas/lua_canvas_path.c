#include "lua_canvas_path.h"
#include "canvas/canvas_methods.h"
#include "lua_resource.h"
#include "utils/cairo.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <math.h>

static void
lua_catnip_canvas_path_move(
  lua_State* L,
  struct catnip_canvas* canvas,
  bool relative
)
{
  lua_rawgeti(L, -1, 2);
  double x = lua_popnumber(L);

  lua_rawgeti(L, -1, 3);
  double y = lua_popnumber(L);

  if (relative) {
    cairo_rel_move_to(canvas->cr, x, y);
  } else {
    cairo_move_to(canvas->cr, x, y);
  }
}

static void
lua_catnip_canvas_path_line(
  lua_State* L,
  struct catnip_canvas* canvas,
  bool relative
)
{
  lua_rawgeti(L, -1, 2);
  double x = lua_popnumber(L);

  lua_rawgeti(L, -1, 3);
  double y = lua_popnumber(L);

  if (relative) {
    cairo_rel_line_to(canvas->cr, x, y);
  } else {
    cairo_line_to(canvas->cr, x, y);
  }
}

static void
lua_catnip_canvas_path_arc(
  lua_State* L,
  struct catnip_canvas* canvas,
  bool relative
)
{
  double x, y;
  cairo_get_current_point(canvas->cr, &x, &y);

  lua_rawgeti(L, -1, 2);
  double cx = lua_popnumber(L);

  lua_rawgeti(L, -1, 3);
  double cy = lua_popnumber(L);

  lua_rawgeti(L, -1, 4);
  double radians = lua_popnumber(L);

  if (relative) {
    cx += x;
    cy += y;
  }

  double radius = sqrt(pow(x - cx, 2) + pow(y - cy, 2));

  double theta = asin((y - cy) / radius);
  theta = x < cx ? M_PI - theta : theta;

  cairo_arc(canvas->cr, cx, cy, radius, theta, theta + radians);
}

static void
lua_catnip_canvas_path_bezier(
  lua_State* L,
  struct catnip_canvas* canvas,
  bool relative
)
{
  lua_rawgeti(L, -1, 2);
  double control1_x = lua_popnumber(L);

  lua_rawgeti(L, -1, 3);
  double control1_y = lua_popnumber(L);

  lua_rawgeti(L, -1, 4);
  double control2_x = lua_popnumber(L);

  lua_rawgeti(L, -1, 5);
  double control2_y = lua_popnumber(L);

  lua_rawgeti(L, -1, 6);
  double x = lua_popnumber(L);

  lua_rawgeti(L, -1, 7);
  double y = lua_popnumber(L);

  if (relative) {
    cairo_rel_curve_to(
      canvas->cr,
      control1_x,
      control1_y,
      control2_x,
      control2_y,
      x,
      y
    );
  } else {
    cairo_curve_to(
      canvas->cr,
      control1_x,
      control1_y,
      control2_x,
      control2_y,
      x,
      y
    );
  }
}

int
lua_catnip_canvas_path(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");

  luaL_checktype(L, 2, LUA_TTABLE);
  cairo_save(canvas->cr);
  cairo_move_to(canvas->cr, 0, 0);

  lua_pushnil(L);
  while (lua_next(L, 2) != 0) {
    if (lua_type(L, -2) == LUA_TNUMBER && lua_type(L, -1) == LUA_TTABLE) {
      lua_rawgeti(L, -1, 1);
      const char* command = lua_popstring(L);

      if (streq(command, "close") || streq(command, "CLOSE")) {
        cairo_close_path(canvas->cr);
      } else if (streq(command, "MOVE")) {
        lua_catnip_canvas_path_move(L, canvas, false);
      } else if (streq(command, "move")) {
        lua_catnip_canvas_path_move(L, canvas, true);
      } else if (streq(command, "LINE")) {
        lua_catnip_canvas_path_line(L, canvas, false);
      } else if (streq(command, "line")) {
        lua_catnip_canvas_path_line(L, canvas, true);
      } else if (streq(command, "ARC")) {
        lua_catnip_canvas_path_arc(L, canvas, false);
      } else if (streq(command, "arc")) {
        lua_catnip_canvas_path_arc(L, canvas, true);
      } else if (streq(command, "BEZIER")) {
        lua_catnip_canvas_path_bezier(L, canvas, false);
      } else if (streq(command, "bezier")) {
        lua_catnip_canvas_path_bezier(L, canvas, true);
      }
    }

    lua_pop(L, 1);
  }

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
