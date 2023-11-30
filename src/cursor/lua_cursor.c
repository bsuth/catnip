#include "lua_cursor.h"
#include "properties.h"
#include <glib.h>
#include <lauxlib.h>

lua_Ref lua_catnip_cursor = LUA_NOREF;

static int
lua_catnip_cursor__index(lua_State* L)
{
  if (lua_type(L, 2) != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_cursor_get_x());
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_cursor_get_y());
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_cursor__newindex(lua_State* L)
{
  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_log(L, "invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    catnip_cursor_set_x(luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    catnip_cursor_set_y(luaL_checknumber(L, 3));
  } else {
    lua_log(L, "invalid key: %s", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_cursor_mt[] = {
  {"__index", lua_catnip_cursor__index},
  {"__newindex", lua_catnip_cursor__newindex},
  {NULL, NULL}
};

void
lua_catnip_cursor_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.cursor");
  luaL_setfuncs(L, lua_catnip_cursor_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.cursor");
  lua_catnip_cursor = luaL_ref(L, LUA_REGISTRYINDEX);
}
