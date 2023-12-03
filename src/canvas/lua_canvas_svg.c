#include "lua_canvas_svg.h"
#include "canvas/canvas.h"
#include "utils/lua.h"
#include <lauxlib.h>
#include <librsvg/rsvg.h>

struct catnip_canvas_svg {
  RsvgHandle* rsvg;
  int x;
  int y;
  int width;
  int height;
};

static void
catnip_canvas_svg_draw(
  struct catnip_canvas* canvas,
  struct catnip_canvas_svg* svg
)
{
  RsvgRectangle viewport = {
    .x = svg->x,
    .y = svg->y,
    .width = svg->width,
    .height = svg->height,
  };

  if (viewport.width == -1 || viewport.height == -1) {
    double width_px, height_px;
    rsvg_handle_get_intrinsic_size_in_pixels(svg->rsvg, &width_px, &height_px);

    if (viewport.width == -1 && viewport.height == -1) {
      viewport.width = width_px;
      viewport.height = height_px;
    } else if (viewport.width == -1) {
      viewport.width = viewport.height * (width_px / height_px);
    } else {
      viewport.height = viewport.width * (width_px / height_px);
    }
  }

  rsvg_handle_render_document(svg->rsvg, canvas->cr, &viewport, NULL);
  catnip_canvas_refresh(canvas);
}

int
lua_catnip_canvas_svg(lua_State* L)
{
  struct catnip_canvas** lua_canvas = lua_touserdata(L, 1);
  struct catnip_canvas* canvas = *lua_canvas;

  RsvgHandle** lua_svg = luaL_checkudata(L, 2, "catnip.svg");
  luaL_checktype(L, 3, LUA_TTABLE);

  struct catnip_canvas_svg canvas_svg = {
    .rsvg = *lua_svg,
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
  };

  lua_hasnumberfield(L, 3, "x") && (canvas_svg.x = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "y") && (canvas_svg.y = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "width") && (canvas_svg.width = lua_popinteger(L));
  lua_hasnumberfield(L, 3, "height") && (canvas_svg.height = lua_popinteger(L));

  catnip_canvas_svg_draw(canvas, &canvas_svg);

  return 0;
}
