#ifndef CATNIP_LUA_RESOURCE_LIST_H
#define CATNIP_LUA_RESOURCE_LIST_H

#include "extensions/lua.h"
#include <wayland-util.h>

struct catnip_lua_resource_list {
  struct wl_list head;
  lua_Ref ref;
};

void
lua_catnip_resource_list_init(lua_State* L);

struct catnip_lua_resource_list*
lua_catnip_resource_list_create(lua_State* L);

void
lua_catnip_resource_list_destroy(
  lua_State* L,
  struct catnip_lua_resource_list* lua_resource_list
);

#endif
