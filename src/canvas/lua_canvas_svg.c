#include "lua_canvas_svg.h"
#include "canvas/canvas_methods.h"
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

static bool
is_svg_filename(const char* filename)
{
  size_t len = strlen(filename);
  return (
    len > 3 && filename[len - 4] == '.' && filename[len - 3] == 's'
    && filename[len - 2] == 'v' && filename[len - 1] == 'g'
  );
}

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
  struct catnip_canvas** lua_canvas = luaL_checkudata(L, 1, "catnip.canvas");
  struct catnip_canvas* canvas = *lua_canvas;

  GError* error = NULL;

  const char* svg = luaL_checkstring(L, 2);
  RsvgHandle* rsvg =
    is_svg_filename(svg)
      ? rsvg_handle_new_from_file(svg, &error)
      : rsvg_handle_new_from_data((const guint8*) svg, strlen(svg), &error);

  if (error != NULL) {
    lua_log_error(L, "bad svg (%s)", error->message);
    g_error_free(error);
    return 0;
  }

  struct catnip_canvas_svg canvas_svg = {
    .rsvg = rsvg,
    .x = 0,
    .y = 0,
    .width = -1,
    .height = -1,
  };

  if (lua_type(L, 3) == LUA_TTABLE) {
    if (lua_hasnumberfield(L, 3, "x")) {
      canvas_svg.x = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "y")) {
      canvas_svg.y = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "width")) {
      canvas_svg.width = lua_popinteger(L);
    }

    if (lua_hasnumberfield(L, 3, "height")) {
      canvas_svg.height = lua_popinteger(L);
    }

    if (lua_hasstringfield(L, 3, "stylesheet")) {
      const char* stylesheet = lua_popstring(L);

      rsvg_handle_set_stylesheet(
        rsvg,
        (const guint8*) stylesheet,
        strlen(stylesheet),
        &error
      );

      if (error != NULL) {
        lua_log_error(L, "bad stylesheet (%s)", error->message);
        g_error_free(error);
        g_object_unref(rsvg);
        return 0;
      }
    }
  }

  catnip_canvas_svg_draw(canvas, &canvas_svg);

  g_object_unref(rsvg);

  return 0;
}
