#include "lua_widget_output.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_output__index(lua_State* L)
{
  struct catnip_lua_widget_output* output = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (catnip_lua_widget_base__index(L, &output->base, key)) {
    return 1;
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_output__newindex(lua_State* L)
{
  struct catnip_lua_widget_output* output = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL || catnip_lua_widget_base__newindex(L, &output->base, key)) {
    return 0;
  }

  return 0;
}

static int
catnip_lua_widget_output__gc(lua_State* L)
{
  struct catnip_lua_widget_output* output = lua_touserdata(L, 1);

  catnip_lua_widget_base_cleanup(L, &output->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_output_mt[] = {
  {"__index", catnip_lua_widget_output__index},
  {"__newindex", catnip_lua_widget_output__newindex},
  {"__gc", catnip_lua_widget_output__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_output_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.output");
  luaL_setfuncs(L, catnip_lua_widget_output_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_output(lua_State* L)
{
  struct catnip_lua_widget_output* output =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_output));
  luaL_setmetatable(L, "catnip.widget.output");

  catnip_lua_widget_base_setup(L, &output->base);
  output->base.type = CATNIP_LUA_WIDGET_OUTPUT;

  if (lua_type(L, 1) == LUA_TTABLE) {}

  return 1;
}

void
catnip_lua_widget_output_draw(
  lua_State* L,
  struct catnip_lua_widget_output* output,
  cairo_t* cr
)
{
}
