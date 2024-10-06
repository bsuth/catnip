#include "lua_widget_text.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_text__index(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (catnip_lua_widget_base__index(L, &text->base, key)) {
    return 1;
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_text__newindex(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL || catnip_lua_widget_base__newindex(L, &text->base, key)) {
    return 0;
  }

  return 0;
}

static int
catnip_lua_widget_text__gc(lua_State* L)
{
  struct catnip_lua_widget_text* text = lua_touserdata(L, 1);

  catnip_lua_widget_base_cleanup(L, &text->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_text_mt[] = {
  {"__index", catnip_lua_widget_text__index},
  {"__newindex", catnip_lua_widget_text__newindex},
  {"__gc", catnip_lua_widget_text__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_text_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.text");
  luaL_setfuncs(L, catnip_lua_widget_text_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_text(lua_State* L)
{
  struct catnip_lua_widget_text* text =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_text));
  luaL_setmetatable(L, "catnip.widget.text");

  catnip_lua_widget_base_setup(L, &text->base);
  text->base.type = CATNIP_LUA_WIDGET_TEXT;

  if (lua_type(L, 1) == LUA_TTABLE) {}

  return 1;
}

void
catnip_lua_widget_text_draw(
  lua_State* L,
  struct catnip_lua_widget_text* text,
  cairo_t* cr
)
{
}
