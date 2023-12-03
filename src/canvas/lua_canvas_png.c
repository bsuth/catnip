#include "lua_canvas_png.h"
#include "canvas/canvas.h"
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

  int surface_width = cairo_image_surface_get_width(png->cairo_surface);
  int surface_height = cairo_image_surface_get_height(png->cairo_surface);

  if (png->width != -1 || png->height != -1) {
    double scale_x = (double) png->width / surface_width;
    double scale_y = (double) png->height / surface_height;

    cairo_scale(
      canvas->cr,
      png->width != -1 ? scale_x : scale_y,
      png->height != -1 ? scale_y : scale_x
    );
  }

  cairo_set_source_surface(canvas->cr, png->cairo_surface, png->x, png->y);
  cairo_rectangle(canvas->cr, 0, 0, surface_width, surface_height);
  cairo_fill(canvas->cr);

  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_png(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  cairo_surface_t** lua_png = luaL_checkudata(L, 2, "catnip.png");
  luaL_checktype(L, 3, LUA_TTABLE);

  struct catnip_canvas_png canvas_png = {
    .cairo_surface = *lua_png,
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
  };

  lua_hasnumberfield(L, 3, "x") && (canvas_png.x = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "y") && (canvas_png.y = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "width") && (canvas_png.width = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "height") && (canvas_png.height = lua_popinteger(L));

  catnip_canvas_png_draw(canvas, &canvas_png);

  return 0;
}
