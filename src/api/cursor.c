#include "cursor.h"
#include "api/api.h"
#include "server/cursor.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

lua_Ref api_catnip_cursor = LUA_NOREF;

// -----------------------------------------------------------------------------
// Metatable: catnip.cursor
// -----------------------------------------------------------------------------

static int
api_cursor__index(lua_State* L)
{
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "lx")) {
    lua_pushnumber(L, server_cursor_get_lx());
  } else if (g_str_equal(key, "ly")) {
    lua_pushnumber(L, server_cursor_get_ly());
  } else if (g_str_equal(key, "gx")) {
    lua_pushnumber(L, server_cursor_get_gx());
  } else if (g_str_equal(key, "gy")) {
    lua_pushnumber(L, server_cursor_get_gy());
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_cursor__newindex(lua_State* L)
{
  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    log_warning("invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "lx")) {
    server_cursor_set_lx(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "ly")) {
    server_cursor_set_ly(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "gx")) {
    server_cursor_set_gx(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "gy")) {
    server_cursor_set_gy(luaL_checknumber(L, 3));
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static const struct luaL_Reg api_cursor_metatable[] = {
  {"__index", api_cursor__index},
  {"__newindex", api_cursor__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

void
init_api_cursor(lua_State* L)
{
  luaL_newmetatable(L, "catnip.cursor");
  luaL_setfuncs(L, api_cursor_metatable, 0);
  lua_pop(L, 1);

  struct lua_window* lua_window = lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.cursor");
  api_catnip_cursor = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_cursor);
  lua_setfield(L, -2, "cursor");
  lua_pop(L, 1);
}
