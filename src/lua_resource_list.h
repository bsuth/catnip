#ifndef CATNIP_LUA_RESOURCE_LIST_H
#define CATNIP_LUA_RESOURCE_LIST_H

#include "lua_resource.h"
#include "utils/lua.h"
#include <wayland-util.h>

typedef void (*catnip_lua_resource_list__destroy)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

struct catnip_lua_resource_list {
  struct wl_list head;
  lua_Ref ref;
  catnip_lua_resource_list__destroy __destroy;
};

struct catnip_lua_resource_list*
lua_catnip_resource_list_create(lua_State* L);

void
lua_catnip_resource_list_destroy(
  lua_State* L,
  struct catnip_lua_resource_list* lua_resource_list
);

void
lua_catnip_resource_list_init(lua_State* L);

#endif
