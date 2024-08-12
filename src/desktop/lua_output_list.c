#include "lua_output_list.h"
#include "desktop/lua_output.h"
#include "desktop/outputs.h"
#include <lauxlib.h>

struct catnip_lua_resource_list* lua_catnip_output_list = NULL;

void
lua_catnip_output_list_init(lua_State* L)
{
  lua_catnip_output_list = lua_catnip_resource_list_create(L);
}

void
lua_catnip_output_list_populate(lua_State* L)
{
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    lua_catnip_output_create(L, output);
  }
}
