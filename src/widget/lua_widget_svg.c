#include "lua_widget_svg.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_svg__index(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (catnip_lua_widget_base__index(L, &svg->base, key)) {
    return 1;
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_svg__newindex(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL || catnip_lua_widget_base__newindex(L, &svg->base, key)) {
    return 0;
  }

  return 0;
}

static int
catnip_lua_widget_svg__gc(lua_State* L)
{
  struct catnip_lua_widget_svg* svg = lua_touserdata(L, 1);

  catnip_lua_widget_base_cleanup(L, &svg->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_svg_mt[] = {
  {"__index", catnip_lua_widget_svg__index},
  {"__newindex", catnip_lua_widget_svg__newindex},
  {"__gc", catnip_lua_widget_svg__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_svg_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.svg");
  luaL_setfuncs(L, catnip_lua_widget_svg_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_svg(lua_State* L)
{
  struct catnip_lua_widget_svg* svg =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_svg));
  luaL_setmetatable(L, "catnip.widget.svg");

  catnip_lua_widget_base_setup(L, &svg->base);
  svg->base.type = CATNIP_LUA_WIDGET_SVG;

  if (lua_type(L, 1) == LUA_TTABLE) {}

  return 1;
}

void
catnip_lua_widget_svg_draw(
  lua_State* L,
  struct catnip_lua_widget_svg* svg,
  cairo_t* cr
)
{
}
