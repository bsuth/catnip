#include "lua_canvas_methods.h"
#include "canvas/canvas_methods.h"
#include <lauxlib.h>

int
lua_catnip_canvas_clear(lua_State* L)
{
  struct catnip_canvas** lua_canvas = luaL_checkudata(L, 1, "catnip.canvas");
  catnip_canvas_clear(*lua_canvas);
  return 0;
}
