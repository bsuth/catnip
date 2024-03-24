#include "lua_canvas_png.h"
#include "canvas/canvas_methods.h"
#include "lua_resource.h"
#include "utils/lua.h"
#include <lauxlib.h>

int
lua_catnip_canvas_png(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkname(L, 1, "canvas");

  const char* filename = luaL_checkstring(L, 2);
  cairo_surface_t* png_surface = cairo_image_surface_create_from_png(filename);

  double x = 0;
  double y = 0;

  double scale_x = 1;
  double scale_y = 1;

  if (lua_type(L, 3) == LUA_TTABLE) {
    x = lua_hasnumberfield(L, 3, "x") ? lua_popnumber(L) : x;
    y = lua_hasnumberfield(L, 3, "y") ? lua_popnumber(L) : y;

    if (lua_hasnumberfield(L, 3, "width")) {
      double png_surface_width = cairo_image_surface_get_width(png_surface);
      scale_x = lua_popnumber(L) / png_surface_width;
    }

    if (lua_hasnumberfield(L, 3, "height")) {
      double png_surface_height = cairo_image_surface_get_height(png_surface);
      scale_y = lua_popnumber(L) / png_surface_height;
    }
  }

  scale_x = scale_x != 1 ? scale_x : scale_y;
  scale_y = scale_y != 1 ? scale_y : scale_x;

  cairo_save(canvas->cr);
  cairo_scale(canvas->cr, scale_x, scale_y);
  cairo_set_source_surface(canvas->cr, png_surface, x / scale_x, y / scale_y);
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);

  cairo_surface_destroy(png_surface);

  return 0;
}
