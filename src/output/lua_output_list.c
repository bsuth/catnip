#include "lua_output_list.h"
#include "output/lua_output.h"
#include <lauxlib.h>

struct catnip_lua_resource_list* lua_catnip_output_list = NULL;

void
lua_catnip_output_list_init(lua_State* L)
{
  lua_catnip_output_list = lua_catnip_resource_list_create(L);
  lua_catnip_output_list->__destroy = lua_catnip_output_destroy;

  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    lua_catnip_output_create(L, output);
  }
}
