#include "lua_png.h"
#include "utils/log.h"
#include <cairo.h>
#include <lauxlib.h>

static int
lua_catnip_png__gc(lua_State* L)
{
  cairo_surface_t** lua_png = lua_touserdata(L, 1);
  cairo_surface_destroy(*lua_png);
  return 0;
}

static const struct luaL_Reg lua_catnip_png_metatable[] = {
  {"__gc", lua_catnip_png__gc},
  {NULL, NULL}
};

void
lua_catnip_png_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.png");
  luaL_setfuncs(L, lua_catnip_png_metatable, 0);
  lua_pop(L, 1);
}

int
lua_catnip_png_create(lua_State* L)
{
  const char* filename = luaL_checkstring(L, 1);
  cairo_surface_t* png = cairo_image_surface_create_from_png(filename);

  if (png == NULL) {
    log_error("failed to load png");
    lua_pushnil(L);
    return 1;
  }

  cairo_surface_t** lua_png = lua_newuserdata(L, sizeof(cairo_surface_t*));
  luaL_setmetatable(L, "catnip.png");
  *lua_png = png;

  return 1;
}
