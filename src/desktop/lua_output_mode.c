#include "lua_output_mode.h"
#include "extensions/string.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// __index
// -----------------------------------------------------------------------------

static int
catnip_lua_output_mode__index(lua_State* L)
{
  struct catnip_lua_output_mode* lua_output_mode = lua_touserdata(L, 1);
  const char* key = lua_tostring(L, 2);

  if (streq(key, "width")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->height);
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->refresh);
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

static const struct luaL_Reg catnip_lua_output_mode_mt[] = {
  {"__index", catnip_lua_output_mode__index},
  {NULL, NULL}
};

void
catnip_lua_output_mode_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.output.mode");
  luaL_setfuncs(L, catnip_lua_output_mode_mt, 0);
  lua_pop(L, 1);
}

struct catnip_lua_output_mode*
catnip_lua_output_mode_create(
  lua_State* L,
  struct wlr_output_mode* wlr_output_mode
)
{
  struct catnip_lua_output_mode* lua_output_mode =
    lua_newuserdata(L, sizeof(struct catnip_lua_output_mode));
  luaL_setmetatable(L, "catnip.output.mode");

  lua_output_mode->ref = luaL_ref(L, LUA_REGISTRYINDEX);
  lua_output_mode->wlr_output_mode = wlr_output_mode;

  return lua_output_mode;
}

void
catnip_lua_output_mode_destroy(
  lua_State* L,
  struct catnip_lua_output_mode* lua_output_mode
)
{
  luaL_unref(L, LUA_REGISTRYINDEX, lua_output_mode->ref);
}
