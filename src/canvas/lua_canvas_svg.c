#include "lua_canvas_svg.h"
#include "canvas/canvas.h"
#include "canvas/canvas_cache.h"
#include "canvas/canvas_svg.h"
#include "lua_resource.h"
#include <lauxlib.h>

int
catnip_lua_canvas_svg(lua_State* L)
{
  struct catnip_canvas* canvas = catnip_lua_resource_checkname(L, 1, "canvas");
  const char* document = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TTABLE);

  bool ignore_cache = lua_hasfield(L, 3, "cache") && !lua_popboolean(L);
  const char* styles = lua_hasfield(L, 3, "styles") ? lua_popstring(L) : NULL;

  struct catnip_canvas_svg* svg = ignore_cache
    ? catnip_canvas_svg_create(document, styles)
    : catnip_canvas_cache_svg(canvas->cache, document, styles);

  if (svg->rsvg != NULL) {
    double x = lua_hasfield(L, 3, "x") ? lua_popnumber(L) : 0;
    double y = lua_hasfield(L, 3, "y") ? lua_popnumber(L) : 0;
    double width = lua_hasfield(L, 3, "width") ? lua_popnumber(L) : -1;
    double height = lua_hasfield(L, 3, "height") ? lua_popnumber(L) : -1;

    RsvgRectangle viewport = {
      .x = x,
      .y = y,
      .width = width == -1 ? svg->width : width,
      .height = height == -1 ? svg->height : height,
    };

    if (width != -1 && height == -1) {
      viewport.height *= width / svg->width;
    } else if (width == -1 && height != -1) {
      viewport.width *= height / svg->height;
    }

    rsvg_handle_render_document(svg->rsvg, canvas->cr, &viewport, NULL);
    catnip_canvas_refresh(canvas);
  }

  if (ignore_cache) {
    catnip_canvas_svg_destroy(svg);
  }

  return 0;
}
