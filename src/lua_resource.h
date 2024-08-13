#ifndef CATNIP_LUA_RESOURCE_H
#define CATNIP_LUA_RESOURCE_H

#include "extensions/lua.h"
#include <wayland-util.h>

struct catnip_lua_resource;

typedef bool (*catnip_lua_resource_index)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
);

typedef bool (*catnip_lua_resource_newindex)(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* key
);

typedef void (*catnip_lua_resource_gc)(
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

  catnip_lua_resource_index __index;
  catnip_lua_resource_newindex __newindex;
  catnip_lua_resource_gc __gc;
};

void
catnip_lua_resource_init(lua_State* L);

struct catnip_lua_resource*
catnip_lua_resource_create(lua_State* L);

void
catnip_lua_resource_destroy(
  lua_State* L,
  struct catnip_lua_resource* lua_resource
);

void*
catnip_lua_resource_checkname(lua_State* L, int index, const char* name);

void
catnip_lua_resource_publish(
  lua_State* L,
  struct catnip_lua_resource* lua_resource,
  const char* event,
  int nargs
);

#endif
