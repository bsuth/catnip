#ifndef CATNIP_LUA_OBJECTS_LUA_OBJECT_LIST_H
#define CATNIP_LUA_OBJECTS_LUA_OBJECT_LIST_H

#include "extensions/lua.h"
#include <lua.h>

typedef struct _catnip_lua_object_list catnip_lua_object_list_t;

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_lua_object_list_t*
catnip_lua_object_list_create(lua_State* L);

void
catnip_lua_object_list_destroy(
  lua_State* L,
  catnip_lua_object_list_t* lua_object_list
);

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

lua_Ref
catnip_lua_object_list_get_ref(
  lua_State* L,
  catnip_lua_object_list_t* lua_object_list
);

#endif
