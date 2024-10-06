#include "lua_widget_window.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_window__index(lua_State* L)
{
  struct catnip_lua_widget_window* window = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (catnip_lua_widget_base__index(L, &window->base, key)) {
    return 1;
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_widget_window__newindex(lua_State* L)
{
  struct catnip_lua_widget_window* window = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (key == NULL || catnip_lua_widget_base__newindex(L, &window->base, key)) {
    return 0;
  }

  return 0;
}

static int
catnip_lua_widget_window__gc(lua_State* L)
{
  struct catnip_lua_widget_window* window = lua_touserdata(L, 1);

  catnip_lua_widget_base_cleanup(L, &window->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_window_mt[] = {
  {"__index", catnip_lua_widget_window__index},
  {"__newindex", catnip_lua_widget_window__newindex},
  {"__gc", catnip_lua_widget_window__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_window_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.window");
  luaL_setfuncs(L, catnip_lua_widget_window_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_window(lua_State* L)
{
  struct catnip_lua_widget_window* window =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_window));
  luaL_setmetatable(L, "catnip.widget.window");

  catnip_lua_widget_base_setup(L, &window->base);
  window->base.type = CATNIP_LUA_WIDGET_WINDOW;

  if (lua_type(L, 1) == LUA_TTABLE) {}

  return 1;
}

void
catnip_lua_widget_window_draw(
  lua_State* L,
  struct catnip_lua_widget_window* window,
  cairo_t* cr
)
{
}
