#include "lua_output_mode_list.h"
#include "lua_resource_list.h"
#include "output/lua_output_mode.h"
#include <lauxlib.h>

struct catnip_lua_resource_list*
lua_catnip_output_mode_list_create(lua_State* L, struct catnip_output* output)
{
  struct catnip_lua_resource_list* lua_resource_list =
    lua_catnip_resource_list_create(L);

  lua_resource_list->__destroy = lua_catnip_output_mode_destroy;

  struct wlr_output_mode* output_mode = NULL;
  wl_list_for_each(output_mode, &output->wlr_output->modes, link)
  {
    wl_list_insert(
      &lua_resource_list->head,
      &lua_catnip_output_mode_create(L, output_mode)->link
    );
  }

  return lua_resource_list;
}
