#include "lua_canvas_png.h"
#include "canvas/canvas_methods.h"
#include "utils/lua.h"
#include <lauxlib.h>

struct catnip_canvas_png {
  cairo_surface_t* cairo_surface;
  int x;
  int y;
  int width;
  int height;
};

static void
catnip_canvas_png_draw(
  struct catnip_canvas* canvas,
  struct catnip_canvas_png* png
)
{
  cairo_save(canvas->cr);

  int x = png->x;
  int y = png->y;

  double scale_x =
    (double) png->width / cairo_image_surface_get_width(png->cairo_surface);
  double scale_y =
    (double) png->height / cairo_image_surface_get_height(png->cairo_surface);

  if (png->width != -1 && png->height != -1) {
    x /= scale_x;
    y /= scale_y;
    cairo_scale(canvas->cr, scale_x, scale_y);
  } else if (png->width != -1) {
    x /= scale_x;
    y /= scale_x;
    cairo_scale(canvas->cr, scale_x, scale_x);
  } else if (png->height != -1) {
    x /= scale_y;
    y /= scale_y;
    cairo_scale(canvas->cr, scale_y, scale_y);
  }

  cairo_set_source_surface(canvas->cr, png->cairo_surface, x, y);
  cairo_paint(canvas->cr);

  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_png(lua_State* L)
{
  struct catnip_canvas** lua_canvas = luaL_checkudata(L, 1, "catnip.canvas");
  struct catnip_canvas* canvas = *lua_canvas;

  const char* filename = luaL_checkstring(L, 2);

  struct catnip_canvas_png canvas_png = {
    .cairo_surface = cairo_image_surface_create_from_png(filename),
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
  };

  if (lua_type(L, 3) == LUA_TTABLE) {
    if (lua_hasnumberfield(L, 3, "x")) {
      canvas_png.x = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "y")) {
      canvas_png.y = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "width")) {
      canvas_png.width = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "height")) {
      canvas_png.height = lua_popinteger(L);
    }
  }

  catnip_canvas_png_draw(canvas, &canvas_png);

  cairo_surface_destroy(canvas_png.cairo_surface);

  return 0;
}
