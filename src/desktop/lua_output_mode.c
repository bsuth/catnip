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

  if (lua_output_mode->wlr_output_mode == NULL) {
    lua_log_error(L, "attempt to index outdated output mode");
    lua_pushnil(L);
  } else if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "width")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->height);
  } else if (streq(key, "refresh")) {
    lua_pushnumber(L, lua_output_mode->wlr_output_mode->refresh);
  } else {
    lua_pushnil(L);
  }

  return 1;
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
  // Explicitly set `NULL`, just in case the user is keeping a reference.
  lua_output_mode->wlr_output_mode = NULL;

  luaL_unref(L, LUA_REGISTRYINDEX, lua_output_mode->ref);
  wl_list_remove(&lua_output_mode->link);
}
