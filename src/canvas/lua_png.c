#include "lua_png.h"
#include "canvas/canvas_methods.h"
#include "utils/lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

static void
lua_catnip_png_reload(lua_State* L, struct lua_catnip_png* lua_png)
{
  lua_png->surface = cairo_image_surface_create_from_png(lua_png->path);
  lua_png->width = cairo_image_surface_get_width(lua_png->surface);
  lua_png->height = cairo_image_surface_get_height(lua_png->surface);
}

static int
lua_catnip_png__index(lua_State* L)
{
  struct lua_catnip_png* lua_png = lua_touserdata(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "path")) {
    lua_pushstring(L, lua_png->path);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_png__newindex(lua_State* L)
{
  struct lua_catnip_png* lua_png = lua_touserdata(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_log_warning(
      L,
      "attempt to index resource with type '%s'",
      luaL_typename(L, 2)
    );
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "path")) {
    const char* path = luaL_checkstring(L, 3);
    free(lua_png->path);
    lua_png->path = strdup(path);
    lua_catnip_png_reload(L, lua_png);
  } else {
    lua_log_warning(L, "attempt to set unknown index '%s'", key);
  }

  return 0;
}

static int
lua_catnip_png__gc(lua_State* L)
{
  struct lua_catnip_png* lua_png = lua_touserdata(L, 1);
  free(lua_png->path);
  cairo_surface_destroy(lua_png->surface);
  return 0;
}

static const struct luaL_Reg lua_catnip_png_mt[] = {
  {"__index", lua_catnip_png__index},
  {"__newindex", lua_catnip_png__newindex},
  {"__gc", lua_catnip_png__gc},
  {NULL, NULL}
};

int
lua_catnip_png(lua_State* L)
{
  const char* path = luaL_checkstring(L, 1);

  struct lua_catnip_png* lua_png =
    lua_newuserdata(L, sizeof(struct lua_catnip_png));
  luaL_setmetatable(L, "catnip.png");

  lua_png->path = strdup(path);
  lua_catnip_png_reload(L, lua_png);

  return 1;
}

void
lua_catnip_png_render(
  struct lua_catnip_png* lua_png,
  struct catnip_canvas* canvas,
  double x,
  double y,
  double width,
  double height
)
{
  double scale_x = width != -1 ? width / lua_png->width : 1;
  double scale_y = height != -1 ? height / lua_png->height : 1;

  if (width != -1 && height == -1) {
    scale_y = scale_x;
  } else if (width == -1 && height != -1) {
    scale_x = scale_y;
  }

  cairo_save(canvas->cr);
  cairo_scale(canvas->cr, scale_x, scale_y);
  cairo_set_source_surface(
    canvas->cr,
    lua_png->surface,
    x / scale_x,
    y / scale_y
  );
  cairo_paint(canvas->cr);
  cairo_restore(canvas->cr);
  catnip_canvas_refresh(canvas);
}

void
lua_catnip_png_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.png");
  luaL_setfuncs(L, lua_catnip_png_mt, 0);
  lua_pop(L, 1);
}
