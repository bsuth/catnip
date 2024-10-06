#include "lua_widget_image.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_image__index(lua_State* L)
{
  struct catnip_lua_widget_image* image = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (catnip_lua_widget_base__index(L, &image->base, key)) {
    return 1;
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_image__newindex(lua_State* L)
{
  struct catnip_lua_widget_image* image = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL || catnip_lua_widget_base__newindex(L, &image->base, key)) {
    return 0;
  }

  return 0;
}

static int
catnip_lua_widget_image__gc(lua_State* L)
{
  struct catnip_lua_widget_image* image = lua_touserdata(L, 1);

  catnip_lua_widget_base_cleanup(L, &image->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_image_mt[] = {
  {"__index", catnip_lua_widget_image__index},
  {"__newindex", catnip_lua_widget_image__newindex},
  {"__gc", catnip_lua_widget_image__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_image_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.image");
  luaL_setfuncs(L, catnip_lua_widget_image_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_image(lua_State* L)
{
  struct catnip_lua_widget_image* image =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_image));
  luaL_setmetatable(L, "catnip.widget.image");

  catnip_lua_widget_base_setup(L, &image->base);
  image->base.type = CATNIP_LUA_WIDGET_IMAGE;

  if (lua_type(L, 1) == LUA_TTABLE) {}

  return 1;
}

void
catnip_lua_widget_image_draw(
  lua_State* L,
  struct catnip_lua_widget_image* image,
  cairo_t* cr
)
{
}
