#include "lua_output_list.h"
#include "desktop/lua_output.h"
#include "desktop/outputs.h"
#include <lauxlib.h>

struct catnip_lua_resource_list* catnip_lua_output_list = NULL;

void
catnip_lua_output_list_init(lua_State* L)
{
  catnip_lua_output_list = catnip_lua_resource_list_create(L);
}

void
catnip_lua_output_list_populate(lua_State* L)
{
  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    catnip_lua_output_create(L, output);
  }
}
