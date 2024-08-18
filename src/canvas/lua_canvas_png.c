#include "lua_canvas_png.h"
#include "canvas/canvas.h"
#include "canvas/canvas_cache.h"
#include "canvas/canvas_png.h"
#include "extensions/lua.h"
#include <cairo.h>
#include <lauxlib.h>

int
catnip_lua_canvas_png(lua_State* L)
{
  struct catnip_canvas* canvas = luaL_checkudata(L, 1, "catnip.canvas");
  const char* path = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TTABLE);

  bool ignore_cache = lua_hasfield(L, 3, "cache") && !lua_popboolean(L);

  struct catnip_canvas_png* png = ignore_cache
    ? catnip_canvas_png_create(path)
    : catnip_canvas_cache_png(canvas->cache, path);

  if (png->surface != NULL) {
    double x = lua_hasfield(L, 3, "x") ? lua_popnumber(L) : 0;
    double y = lua_hasfield(L, 3, "y") ? lua_popnumber(L) : 0;
    double width = lua_hasfield(L, 3, "width") ? lua_popnumber(L) : -1;
    double height = lua_hasfield(L, 3, "height") ? lua_popnumber(L) : -1;

    double scale_x = width != -1 ? width / png->width : 1;
    double scale_y = height != -1 ? height / png->height : 1;

    if (width != -1 && height == -1) {
      scale_y = scale_x;
    } else if (width == -1 && height != -1) {
      scale_x = scale_y;
    }

    cairo_save(canvas->cairo.cr);
    cairo_scale(canvas->cairo.cr, scale_x, scale_y);
    cairo_set_source_surface(
      canvas->cairo.cr,
      png->surface,
      x / scale_x,
      y / scale_y
    );
    cairo_paint(canvas->cairo.cr);
    cairo_restore(canvas->cairo.cr);
    catnip_canvas_refresh(canvas);
  }

  if (ignore_cache) {
    catnip_canvas_png_destroy(png);
  }

  return 0;
}
