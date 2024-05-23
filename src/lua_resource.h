#ifndef CATNIP_LUA_RESOURCE_H
#define CATNIP_LUA_RESOURCE_H

#include "utils/lua.h"
#include <wayland-util.h>

struct catnip_lua_resource;

typedef bool (*catnip_lua_resource__index)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
);

typedef bool (*catnip_lua_resource__newindex)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
);

typedef void (*catnip_lua_resource__gc)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

struct catnip_lua_resource {
  struct wl_list link;

  lua_Ref ref;
  lua_Ref subscriptions;
  lua_Ref lua_data;

  void* data;
  const char* name;

  catnip_lua_resource__index __index;
  catnip_lua_resource__newindex __newindex;
  catnip_lua_resource__gc __gc;
};

void
lua_catnip_resource_init(lua_State* L);

struct catnip_lua_resource*
lua_catnip_resource_create(lua_State* L);

void
lua_catnip_resource_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

void*
lua_catnip_resource_checkname(lua_State* L, int index, const char* name);

void
lua_catnip_resource_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
);

#endif
