#include "lua_canvas_rectangle.h"
#include "canvas/canvas_methods.h"
#include "utils/cairo.h"
#include "utils/lua.h"
#include <lauxlib.h>

#define default(a, b) a != -1 ? a : b

struct catnip_canvas_rectangle {
  int x;
  int y;
  int width;
  int height;

  int radius_top_left;
  int radius_top_right;
  int radius_bottom_right;
  int radius_bottom_left;

  int color;
  double opacity;

  int fill_color;
  double fill_opacity;

  int stroke_color;
  double stroke_opacity;
  int stroke_size;
};

static void
catnip_canvas_rectangle_draw(
  struct catnip_canvas* canvas,
  struct catnip_canvas_rectangle* rectangle
)
{
  cairo_save(canvas->cr);

  cairo_rounded_rectangle(
    canvas->cr,
    rectangle->x,
    rectangle->y,
    rectangle->width,
    rectangle->height,
    rectangle->radius_top_left,
    rectangle->radius_top_right,
    rectangle->radius_bottom_right,
    rectangle->radius_bottom_left
  );

  cairo_clip_preserve(canvas->cr);

  int fill_color = default(rectangle->fill_color, rectangle->color);
  int fill_opacity = default(rectangle->fill_opacity, rectangle->opacity);

  if (fill_color != -1 && fill_opacity > 0) {
    cairo_set_source_hexa(canvas->cr, fill_color, fill_opacity);
    cairo_paint(canvas->cr);
  }

  int stroke_color = default(rectangle->stroke_color, rectangle->color);
  int stroke_opacity = default(rectangle->stroke_opacity, rectangle->opacity);

  if (stroke_color != -1 && stroke_opacity > 0 && rectangle->stroke_size > 0) {
    cairo_set_line_width(canvas->cr, 2 * rectangle->stroke_size);
    cairo_set_source_hexa(canvas->cr, stroke_color, stroke_opacity);
    cairo_stroke_preserve(canvas->cr);
  }

  // Reset the path generated from `cairo_rounded_rectangle`, since all our
  // painting methods above preserve the path.
  cairo_new_path(canvas->cr);

  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_rectangle(lua_State* L)
{
  struct catnip_canvas** lua_canvas = luaL_checkudata(L, 1, "catnip.canvas");
  struct catnip_canvas* canvas = *lua_canvas;

  struct catnip_canvas_rectangle rectangle = {
    .x = luaL_checkinteger(L, 2),
    .y = luaL_checkinteger(L, 3),
    .width = luaL_checkinteger(L, 4),
    .height = luaL_checkinteger(L, 5),

    .radius_top_left = 0,
    .radius_top_right = 0,
    .radius_bottom_right = 0,
    .radius_bottom_left = 0,

    .color = -1,
    .opacity = 1,

    .fill_color = -1,
    .fill_opacity = -1,

    .stroke_color = -1,
    .stroke_opacity = -1,
  };

  luaL_checktype(L, 6, LUA_TTABLE);

  if (lua_hasfield(L, 6, "radius")) {
    int radius_type = lua_type(L, -1);

    if (radius_type == LUA_TNUMBER) {
      int radius = lua_tointeger(L, -1);
      rectangle.radius_top_left = radius;
      rectangle.radius_top_right = radius;
      rectangle.radius_bottom_right = radius;
      rectangle.radius_bottom_left = radius;
    } else if (radius_type == LUA_TTABLE) {
      lua_rawgeti(L, -1, 1);
      rectangle.radius_top_left = lua_popinteger(L);
      lua_rawgeti(L, -1, 2);
      rectangle.radius_top_right = lua_popinteger(L);
      lua_rawgeti(L, -1, 3);
      rectangle.radius_bottom_right = lua_popinteger(L);
      lua_rawgeti(L, -1, 4);
      rectangle.radius_bottom_left = lua_popinteger(L);
    } else {
      lua_log_bad_field_type(L, 6, "radius");
    }

    lua_pop(L, 1);
  }

  if (lua_hasnumberfield(L, 6, "color")) {
    rectangle.color = lua_popinteger(L);
  }

  if (lua_hasnumberfield(L, 6, "opacity")) {
    rectangle.opacity = lua_popnumber(L);
  }

  if (lua_hasnumberfield(L, 6, "fill_color")) {
    rectangle.fill_color = lua_popinteger(L);
  }

  if (lua_hasnumberfield(L, 6, "fill_opacity")) {
    rectangle.fill_opacity = lua_popnumber(L);
  }

  if (lua_hasnumberfield(L, 6, "stroke_color")) {
    rectangle.stroke_color = lua_popinteger(L);
  }

  if (lua_hasnumberfield(L, 6, "stroke_opacity")) {
    rectangle.stroke_opacity = lua_popnumber(L);
  }

  if (lua_hasnumberfield(L, 6, "stroke_size")) {
    rectangle.stroke_size = lua_popinteger(L);
  }

  catnip_canvas_rectangle_draw(canvas, &rectangle);

  return 0;
}
