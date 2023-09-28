#include "server/cursor.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
api_cursor__index(lua_State* L)
{
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "lx")) {
    lua_pushnumber(L, server_cursor_get_lx());
  } else if (g_str_equal(field, "ly")) {
    lua_pushnumber(L, server_cursor_get_ly());
  } else if (g_str_equal(field, "gx")) {
    lua_pushnumber(L, server_cursor_get_gx());
  } else if (g_str_equal(field, "gy")) {
    lua_pushnumber(L, server_cursor_get_gy());
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_cursor__newindex(lua_State* L)
{
  if (lua_type(L, 2) != LUA_TSTRING) {
    // TODO: error?
    return 0;
  }

  const char* field = lua_tostring(L, 2);

  if (g_str_equal(field, "lx")) {
    server_cursor_set_lx(luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "ly")) {
    server_cursor_set_ly(luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "gx")) {
    server_cursor_set_gx(luaL_checknumber(L, 3));
  } else if (g_str_equal(field, "gy")) {
    server_cursor_set_gy(luaL_checknumber(L, 3));
  } else {
    // TODO: error?
  }

  return 0;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

static const struct luaL_Reg lua_cursor_metatable[] = {
  {"__index", api_cursor__index},
  {"__newindex", api_cursor__newindex},
  {NULL, NULL}};

void
init_api_cursor(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, "catnip");

  struct lua_window* lua_window = lua_newuserdata(L, 0);

  luaL_newmetatable(L, "catnip.cursor");
  luaL_setfuncs(L, lua_cursor_metatable, 0);
  lua_setmetatable(L, -2);

  lua_setfield(L, -2, "cursor");
  lua_pop(L, 3);
}
