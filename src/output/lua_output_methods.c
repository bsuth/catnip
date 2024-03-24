#include "lua_output_methods.h"
#include "lua_resource.h"
#include "output/output.h"
#include "output/output_layout.h"
#include "output/output_methods.h"
#include <lauxlib.h>

int
lua_catnip_output_method_move(lua_State* L)
{
  struct catnip_output* output = lua_catnip_resource_checkname(L, 1, "output");

  wlr_output_layout_add(
    catnip_output_layout,
    output->wlr_output,
    luaL_checknumber(L, 2),
    luaL_checknumber(L, 3)
  );

  return 0;
}

int
lua_catnip_output_method_resize(lua_State* L)
{
  struct catnip_output* output = lua_catnip_resource_checkname(L, 1, "output");

  catnip_output_configure(
    output,
    luaL_checknumber(L, 2),
    luaL_checknumber(L, 3),
    -1
  );

  return 0;
}
