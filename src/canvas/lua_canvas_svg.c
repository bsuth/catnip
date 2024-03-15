#include "lua_canvas_svg.h"
#include "canvas/canvas_methods.h"
#include "lua_resource.h"
#include "utils/lua.h"
#include <lauxlib.h>
#include <librsvg/rsvg.h>

static bool
is_svg_filename(const char* filename)
{
  size_t len = strlen(filename);
  return len > 3 && filename[len - 4] == '.' && filename[len - 3] == 's'
    && filename[len - 2] == 'v' && filename[len - 1] == 'g';
}

int
lua_catnip_canvas_svg(lua_State* L)
{
  struct catnip_canvas* canvas = lua_catnip_resource_checkmethod(L, "canvas");
  GError* error = NULL;

  const char* svg = luaL_checkstring(L, 2);
  RsvgHandle* rsvg = is_svg_filename(svg)
    ? rsvg_handle_new_from_file(svg, &error)
    : rsvg_handle_new_from_data((const guint8*) svg, strlen(svg), &error);

  if (error != NULL) {
    lua_log_error(L, "bad svg (%s)", error->message);
    g_error_free(error);
    return 0;
  }

  double scale_x = 1;
  double scale_y = 1;

  RsvgRectangle viewport = {
    .x = 0,
    .y = 0,
    .width = 0,
    .height = 0,
  };

  rsvg_handle_get_intrinsic_size_in_pixels(
    rsvg,
    &viewport.width,
    &viewport.height
  );

  if (lua_type(L, 3) == LUA_TTABLE) {
    viewport.x = lua_hasnumberfield(L, 3, "x") ? lua_popnumber(L) : viewport.x;
    viewport.y = lua_hasnumberfield(L, 3, "y") ? lua_popnumber(L) : viewport.y;

    if (lua_hasnumberfield(L, 3, "width")) {
      scale_x = lua_popnumber(L) / viewport.width;
    }

    if (lua_hasnumberfield(L, 3, "height")) {
      scale_y = lua_popnumber(L) / viewport.height;
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

  scale_x = scale_x != 1 ? scale_x : scale_y;
  scale_y = scale_y != 1 ? scale_y : scale_x;

  viewport.width *= scale_x;
  viewport.height *= scale_y;

  rsvg_handle_render_document(rsvg, canvas->cr, &viewport, NULL);
  catnip_canvas_refresh(canvas);

  g_object_unref(rsvg);

  return 0;
}
