#include "lua_canvas_shapes.h"
#include "canvas/canvas.h"
#include "utils/cairo.h"
#include "utils/lua.h"
#include <lauxlib.h>

struct catnip_canvas_rectangle {
  int x;
  int y;
  int width;
  int height;
  int bg;
  int border;
  int border_width;
  int radius;
  double opacity;
};

static void
catnip_canvas_rectangle_draw(
  struct catnip_canvas* canvas,
  struct catnip_canvas_rectangle* rectangle
)
{
  cairo_t* cr = canvas->cr;
  cairo_save(cr);

  if (rectangle->bg != -1) {
    cairo_set_source_hexa(cr, rectangle->bg, rectangle->opacity);

    int x = rectangle->x + rectangle->border_width;
    int y = rectangle->y + rectangle->border_width;
    int width = rectangle->width - 2 * rectangle->border_width;
    int height = rectangle->height - 2 * rectangle->border_width;

    rectangle->radius == 0
      ? cairo_rectangle(cr, x, y, width, height)
      : cairo_rounded_rectangle(cr, x, y, width, height, rectangle->radius);

    cairo_fill(cr);
  }

  if (rectangle->border != -1) {
    cairo_set_line_width(cr, rectangle->border_width);
    cairo_set_source_hexa(cr, rectangle->border, rectangle->opacity);

    int x = rectangle->x + (double) rectangle->border_width / 2;
    int y = rectangle->y + (double) rectangle->border_width / 2;
    int width = rectangle->width - rectangle->border_width;
    int height = rectangle->height - rectangle->border_width;
    int radius = rectangle->radius + rectangle->border_width;

    rectangle->radius == 0
      ? cairo_rectangle(cr, x, y, width, height)
      : cairo_rounded_rectangle(cr, x, y, width, height, radius);

    cairo_stroke(cr);
  }

  cairo_restore(cr);
  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_rectangle(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  struct catnip_canvas_rectangle rectangle = {
    .x = luaL_checknumber(L, 2),
    .y = luaL_checknumber(L, 3),
    .width = luaL_checknumber(L, 4),
    .height = luaL_checknumber(L, 5),
    .bg = -1,
    .border = -1,
    .border_width = 0,
    .opacity = 1,
    .radius = 0,
  };

  if (lua_type(L, 6) == LUA_TTABLE) {
    lua_hasnumberfield(L, 6, "bg") && (rectangle.bg = lua_popinteger(L));
    lua_hasnumberfield(L, 6, "border")
      && (rectangle.border = lua_popinteger(L));
    lua_hasnumberfield(L, 6, "opacity")
      && (rectangle.opacity = lua_popnumber(L));
    lua_hasnumberfield(L, 6, "radius")
      && (rectangle.radius = lua_popinteger(L));

    if (lua_hasnumberfield(L, 6, "border_width")) {
      rectangle.border_width = lua_popinteger(L);
    } else if (rectangle.border != -1) {
      rectangle.border_width = 1;
    }
  }

  catnip_canvas_rectangle_draw(canvas, &rectangle);

  return 0;
}
