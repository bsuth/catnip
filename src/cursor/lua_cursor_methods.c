#include "lua_cursor_methods.h"
#include "cursor/cursor_methods.h"
#include "lua_resource.h"
#include <lauxlib.h>

int
lua_catnip_cursor_method_move(lua_State* L)
{
  lua_catnip_resource_checkname(L, 1, "cursor");
  catnip_cursor_move(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
  return 0;
}
