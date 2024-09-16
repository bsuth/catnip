#include "lua_widget_base.h"
#include <drm_fourcc.h>
#include <lauxlib.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_base__index(lua_State* L)
{
  struct catnip_widget_base* base = lua_touserdata(L, 1);

  if (base->mt->__index != NULL) {
    return base->mt->__index(L, base);
  }

  lua_pushnil(L);

  return 1;
}

// -----------------------------------------------------------------------------
// __newindex
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_base__newindex(lua_State* L)
{
  struct catnip_widget_base* base = lua_touserdata(L, 1);

  if (base->mt->__newindex != NULL) {
    base->mt->__newindex(L, base);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// __gc
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_base__gc(lua_State* L)
{
  struct catnip_widget_base* base = lua_touserdata(L, 1);

  if (base->mt->__gc != NULL) {
    base->mt->__gc(L, base);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_widget_base_mt[] = {
  {"__index", catnip_lua_widget_base__index},
  {"__newindex", catnip_lua_widget_base__newindex},
  {"__gc", catnip_lua_widget_base__gc},
  {NULL, NULL}
};

void
catnip_lua_widget_base_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.base");
  luaL_setfuncs(L, catnip_lua_widget_base_mt, 0);
  lua_pop(L, 1);
}

struct catnip_widget_base*
catnip_lua_widget_base(lua_State* L)
{
  struct catnip_widget_base* base =
    lua_newuserdata(L, sizeof(struct catnip_widget_base));
  luaL_setmetatable(L, "catnip.widget.base");

  base->parent = NULL;
  base->data = NULL;
  base->type = -1;
  base->mt = NULL;

  return base;
}
