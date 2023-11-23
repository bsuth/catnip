#include "png.h"
#include "config/lua_catnip.h"
#include "utils/log.h"
#include <cairo.h>
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable: catnip.png
// -----------------------------------------------------------------------------

static int
api_png__gc(lua_State* L)
{
  cairo_surface_t** api_png = lua_touserdata(L, 1);
  cairo_surface_destroy(*api_png);
  return 0;
}

static const struct luaL_Reg api_png_metatable[] = {
  {"__gc", api_png__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// API PNG
// -----------------------------------------------------------------------------

static int
api_png_create(lua_State* L)
{
  const char* filename = luaL_checkstring(L, 1);
  cairo_surface_t* png = cairo_image_surface_create_from_png(filename);

  if (png == NULL) {
    log_error("failed to load png");
    lua_pushnil(L);
    return 1;
  }

  cairo_surface_t** api_png = lua_newuserdata(L, sizeof(cairo_surface_t*));
  luaL_setmetatable(L, "catnip.png");
  *api_png = png;

  return 1;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
api_png_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.png");
  luaL_setfuncs(L, api_png_metatable, 0);
  lua_pop(L, 1);

  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip);
  lua_pushcfunction(L, api_png_create);
  lua_setfield(L, -2, "png");
  lua_pop(L, 1);
}
